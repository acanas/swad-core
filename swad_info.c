// swad_info.c: info about course

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

#include <limits.h>		// For maximum values
#include <linux/limits.h>	// For PATH_MAX, NAME_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For getenv, etc
#include <stdsoap2.h>		// For SOAP_OK and soap functions
#include <string.h>		// For string functions
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_info.h"
#include "swad_parameter.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Public constants ********************************/
/*****************************************************************************/

const char *Inf_FileNamesForInfoType[Inf_NUM_INFO_TYPES] =
  {
   Cfg_CRS_INFO_INTRODUCTION,
   Cfg_CRS_INFO_TEACHING_GUIDE,
   Cfg_CRS_INFO_LECTURES,
   Cfg_CRS_INFO_PRACTICALS,
   Cfg_CRS_INFO_BIBLIOGRAPHY,
   Cfg_CRS_INFO_FAQ,
   Cfg_CRS_INFO_LINKS,
   Cfg_CRS_INFO_ASSESSMENT,
  };

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/* Functions to write forms in course edition (FAQ, links, etc.) */
void (*Inf_FormsForEditionTypes[Inf_NUM_INFO_SOURCES])(Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType) =
  {
   NULL,
   Inf_FormToEnterIntegratedEditor,
   Inf_FormToEnterPlainTextEditor,
   Inf_FormToEnterRichTextEditor,
   Inf_FormToSendPage,
   Inf_FormToSendURL,
  };

const char *Inf_NamesInDBForInfoSrc[Inf_NUM_INFO_SOURCES] =
  {
   "none",
   "editor",
   "plain_text",
   "rich_text",
   "page",
   "URL",
  };
const Act_Action_t Inf_ActionsSeeInfo[Inf_NUM_INFO_TYPES] =
  {
   ActSeeCrsInf,
   ActSeeTchGui,
   ActSeeSylLec,
   ActSeeSylPra,
   ActSeeBib,
   ActSeeFAQ,
   ActSeeCrsLnk,
   ActSeeAss,
  };
const Act_Action_t Inf_ActionsEditInfo[Inf_NUM_INFO_TYPES] =
  {
   ActEdiCrsInf,
   ActEdiTchGui,
   ActEdiSylLec,
   ActEdiSylPra,
   ActEdiBib,
   ActEdiFAQ,
   ActEdiCrsLnk,
   ActEdiAss,
  };
const Act_Action_t Inf_ActionsChangeForceReadInfo[Inf_NUM_INFO_TYPES] =
  {
   ActChgFrcReaCrsInf,
   ActChgFrcReaTchGui,
   ActChgFrcReaSylLec,
   ActChgFrcReaSylPra,
   ActChgFrcReaBib,
   ActChgFrcReaFAQ,
   ActChgFrcReaCrsLnk,
   ActChgFrcReaAss,
  };
const Act_Action_t Inf_ActionsIHaveReadInfo[Inf_NUM_INFO_TYPES] =
  {
   ActChgHavReaCrsInf,
   ActChgHavReaTchGui,
   ActChgHavReaSylLec,
   ActChgHavReaSylPra,
   ActChgHavReaBib,
   ActChgHavReaFAQ,
   ActChgHavReaCrsLnk,
   ActChgHavReaAss,
  };
const Act_Action_t Inf_ActionsSelecInfoSrc[Inf_NUM_INFO_TYPES] =
  {
   ActSelInfSrcCrsInf,
   ActSelInfSrcTchGui,
   ActSelInfSrcSylLec,
   ActSelInfSrcSylPra,
   ActSelInfSrcBib,
   ActSelInfSrcFAQ,
   ActSelInfSrcCrsLnk,
   ActSelInfSrcAss,
  };
const Act_Action_t Inf_ActionsInfo[Inf_NUM_INFO_SOURCES][Inf_NUM_INFO_TYPES] =
  {
   {
    ActUnk,
    ActUnk,
    ActUnk,
    ActUnk,
    ActUnk,
    ActUnk,
    ActUnk,
    ActUnk,
   },
   {
    ActEditorCrsInf,
    ActEditorTchGui,
    ActEditorSylLec,
    ActEditorSylPra,
    ActEditorBib,
    ActEditorFAQ,
    ActEditorCrsLnk,
    ActEditorAss,
   },
   {
    ActPlaTxtEdiCrsInf,
    ActPlaTxtEdiTchGui,
    ActPlaTxtEdiSylLec,
    ActPlaTxtEdiSylPra,
    ActPlaTxtEdiBib,
    ActPlaTxtEdiFAQ,
    ActPlaTxtEdiCrsLnk,
    ActPlaTxtEdiAss,
   },
   {
    ActRchTxtEdiCrsInf,
    ActRchTxtEdiTchGui,
    ActRchTxtEdiSylLec,
    ActRchTxtEdiSylPra,
    ActRchTxtEdiBib,
    ActRchTxtEdiFAQ,
    ActRchTxtEdiCrsLnk,
    ActRchTxtEdiAss,
   },
   {
    ActRcvPagCrsInf,
    ActRcvPagTchGui,
    ActRcvPagSylLec,
    ActRcvPagSylPra,
    ActRcvPagBib,
    ActRcvPagFAQ,
    ActRcvPagCrsLnk,
    ActRcvPagAss,
   },
   {
    ActRcvURLCrsInf,
    ActRcvURLTchGui,
    ActRcvURLSylLec,
    ActRcvURLSylPra,
    ActRcvURLBib,
    ActRcvURLFAQ,
    ActRcvURLCrsLnk,
    ActRcvURLAss,
   }
  };

const Act_Action_t Inf_ActionsEditorInfo[Inf_NUM_INFO_TYPES] =
  {
   ActEditorCrsInf,
   ActEditorTchGui,
   ActEditorSylLec,
   ActEditorSylPra,
   ActEditorBib,
   ActEditorFAQ,
   ActEditorCrsLnk,
   ActEditorAss,
  };
const Act_Action_t Inf_ActionsRcvPlaTxtInfo[Inf_NUM_INFO_TYPES] =
  {
   ActRcvPlaTxtCrsInf,
   ActRcvPlaTxtTchGui,
   ActRcvPlaTxtSylLec,
   ActRcvPlaTxtSylPra,
   ActRcvPlaTxtBib,
   ActRcvPlaTxtFAQ,
   ActRcvPlaTxtCrsLnk,
   ActRcvPlaTxtAss,
  };
const Act_Action_t Inf_ActionsRcvRchTxtInfo[Inf_NUM_INFO_TYPES] =
  {
   ActRcvRchTxtCrsInf,
   ActRcvRchTxtTchGui,
   ActRcvRchTxtSylLec,
   ActRcvRchTxtSylPra,
   ActRcvRchTxtBib,
   ActRcvRchTxtFAQ,
   ActRcvRchTxtCrsLnk,
   ActRcvRchTxtAss,
  };
const Act_Action_t Inf_ActionsRcvPagBibEvEtc[Inf_NUM_INFO_TYPES] =
  {
   ActRcvPagCrsInf,
   ActRcvPagTchGui,
   ActRcvPagSylLec,
   ActRcvPagSylPra,
   ActRcvPagBib,
   ActRcvPagFAQ,
   ActRcvPagCrsLnk,
   ActRcvPagAss,
  };
const Act_Action_t In_ActionsRcvURLBibEvEtc[Inf_NUM_INFO_TYPES] =
  {
   ActRcvURLCrsInf,
   ActRcvURLTchGui,
   ActRcvURLSylLec,
   ActRcvURLSylPra,
   ActRcvURLBib,
   ActRcvURLFAQ,
   ActRcvURLCrsLnk,
   ActRcvURLAss,
  };
const char *Inf_NamesInDBForInfoType[Inf_NUM_INFO_TYPES] =
  {
   "intro",		// TODO: Change this to "introduction"!
   "description",	// TODO: Change this to "guide"!
   "theory",		// TODO: Change this to "lectures"!
   "practices",		// TODO: Change this to "practicals"!
   "bibliography",
   "FAQ",
   "links",
   "assessment",
  };

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Inf_PutFormToEditInfo (Inf_InfoType_t InfoType);
static void Inf_PutFormToForceStdsToReadInfo (Inf_InfoType_t InfoType,bool MustBeRead);
static void Inf_PutFormToConfirmIHaveReadInfo (Inf_InfoType_t InfoType);
static bool Inf_GetMustBeReadFromForm (void);
static bool Inf_GetIfIHaveReadFromForm (void);
static void Inf_SetForceReadIntoDB (Inf_InfoType_t InfoType,bool MustBeRead);
static void Inf_SetIHaveReadIntoDB (Inf_InfoType_t InfoType,bool IHaveRead);

static void Inf_CheckAndShowPage (Inf_InfoType_t InfoType);
static void Inf_CheckAndShowURL (Inf_InfoType_t InfoType);
static void Inf_ShowPage (Inf_InfoType_t InfoType,const char *URL);
static void Inf_ShowTxtInfo (Inf_InfoType_t InfoType);

/*****************************************************************************/
/******** Show course info (theory, practices, bibliography, etc.) ***********/
/*****************************************************************************/

void Inf_ShowInfo (void)
  {
   extern const char *Txt_No_information_available;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   Inf_InfoSrc_t InfoSrc;
   bool MustBeRead;

   /***** Get info source from database *****/
   Inf_GetInfoSrcFromDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,&InfoSrc,&MustBeRead);

   switch (InfoType)
     {
      case Inf_LECTURES:
      case Inf_PRACTICALS:
         Syl_PutFormWhichSyllabus ();
         break;
      default:
	 break;
     }

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_STUDENT:
         /* Put checkbox to force students to read this couse info */
         if (MustBeRead)
            Inf_PutFormToConfirmIHaveReadInfo (InfoType);
         break;
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_SYS_ADM:
         /* Put link (form) to edit this course info */
         Inf_PutFormToEditInfo (InfoType);

         /* Put checkbox to force students to read this couse info */
         if (InfoSrc != Inf_INFO_SRC_NONE)
            Inf_PutFormToForceStdsToReadInfo (InfoType,MustBeRead);
         break;
      default:
         break;
     }

   switch (InfoSrc)
     {
      case Inf_INFO_SRC_NONE:
	 Lay_ShowAlert (Lay_WARNING,Txt_No_information_available);
         break;
      case Inf_INFO_SRC_EDITOR:
         switch (InfoType)
           {
            case Inf_LECTURES:
            case Inf_PRACTICALS:

	       // TODO: Remove the following lines, here only for debug purposes

               /*
               if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
        	 {
                  char QueryDebug[512*1024];
        	  char *HTMLBuffer;
                  Syl_WriteSyllabusIntoHTMLBuffer (InfoType,&HTMLBuffer);
                  sprintf (QueryDebug,"INSERT INTO debug (DebugTime,Txt) VALUES (NOW(),'%s')",HTMLBuffer);
                  DB_QueryINSERT (QueryDebug,"Error inserting in debug table");
                  free ((void *) HTMLBuffer);
        	 }
               */

               Syl_EditSyllabus ();
               break;
            case Inf_INTRODUCTION:
            case Inf_TEACHING_GUIDE:
            case Inf_BIBLIOGRAPHY:
            case Inf_FAQ:
            case Inf_LINKS:
            case Inf_ASSESSMENT:
	       Lay_ShowAlert (Lay_WARNING,Txt_No_information_available);
	       break;
           }
         break;
      case Inf_INFO_SRC_PLAIN_TEXT:
      case Inf_INFO_SRC_RICH_TEXT:
         Inf_ShowTxtInfo (InfoType);
         break;
      case Inf_INFO_SRC_PAGE:
	       // TODO: Remove the following lines, here only for debug purposes

               /*
               if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
        	 {
                  char QueryDebug[512*1024];
        	  char *HTMLBuffer;
                  Inf_WritePageIntoHTMLBuffer (InfoType,&HTMLBuffer);
                  sprintf (QueryDebug,"INSERT INTO debug (DebugTime,Txt) VALUES (NOW(),'%s')",HTMLBuffer);
                  DB_QueryINSERT (QueryDebug,"Error inserting in debug table");
                  free ((void *) HTMLBuffer);
        	 }
               */

         /***** Open file with web page *****/
	 Inf_CheckAndShowPage (InfoType);
         break;
      case Inf_INFO_SRC_URL:
         /***** Check if file with URL exists *****/
	 Inf_CheckAndShowURL (InfoType);
         break;
     }
  }

/*****************************************************************************/
/******************** Put a link (form) to edit course info ******************/
/*****************************************************************************/

static void Inf_PutFormToEditInfo (Inf_InfoType_t InfoType)
  {
   extern const char *The_ClassFormulB[The_NUM_THEMES];
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin-bottom:10px;\">");
   Act_FormStart (Inf_ActionsEditInfo[InfoType]);
   Act_LinkFormSubmit (Txt_Edit,The_ClassFormulB[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("edit",Txt_Edit,Txt_Edit);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********** Put a form (checkbox) to force students to read info *************/
/*****************************************************************************/

static void Inf_PutFormToForceStdsToReadInfo (Inf_InfoType_t InfoType,bool MustBeRead)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Force_students_to_read_this_information;

   fprintf (Gbl.F.Out,"<div class=\"%s\""
	              " style=\"text-align:center; vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);
   Act_FormStart (Inf_ActionsChangeForceReadInfo[InfoType]);
   fprintf (Gbl.F.Out,"<input type=\"checkbox\"");
   if (MustBeRead)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," name=\"MustBeRead\" value=\"Y\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\" />"
                      " %s",
            Gbl.FormId,
            Txt_Force_students_to_read_this_information);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***** Put a form (checkbox) to confirm that I have read a course info *******/
/*****************************************************************************/

static void Inf_PutFormToConfirmIHaveReadInfo (Inf_InfoType_t InfoType)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_I_have_read_this_information;
   bool IHaveRead = Inf_CheckIfIHaveReadInfo (InfoType);

   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">",
            The_ClassFormul[Gbl.Prefs.Theme]);
   Act_FormStart (Inf_ActionsIHaveReadInfo[InfoType]);
   fprintf (Gbl.F.Out,"<input type=\"checkbox\"");
   if (IHaveRead)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," name=\"IHaveRead\" value=\"Y\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\" />"
                      "%s",
            Gbl.FormId,
            Txt_I_have_read_this_information);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************** Check I have read a course info ************************/
/*****************************************************************************/

bool Inf_CheckIfIHaveReadInfo (Inf_InfoType_t InfoType)
  {
   char Query[512];

   /***** Get if info source is already stored in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_info_read"
                  " WHERE UsrCod='%ld' AND CrsCod='%ld' AND InfoType='%s'",
            Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod,Inf_NamesInDBForInfoType[InfoType]);
   return (DB_QueryCOUNT (Query,"can not get if I have read course info") != 0);
  }

/*****************************************************************************/
/********* Get if students must read any info about current course ***********/
/*****************************************************************************/

bool Inf_GetIfIMustReadAnyCrsInfoInThisCrs (void)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   Inf_InfoType_t InfoType;

   /***** Reset must-be-read to false for all info types *****/
   for (InfoType = (Inf_InfoType_t) 0;
	InfoType < Inf_NUM_INFO_TYPES;
	InfoType++)
      Gbl.CurrentCrs.Info.MustBeRead[InfoType] = false;

   /***** Get info types where students must read info *****/
   sprintf (Query,"SELECT InfoType FROM crs_info_src"
                  " WHERE CrsCod='%ld' AND MustBeRead='Y'"
                  " AND InfoType NOT IN"
                  " (SELECT InfoType FROM crs_info_read"
                  " WHERE UsrCod='%ld' AND CrsCod='%ld')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get if you must read any course info");

   /***** Set must-be-read to true for each rown in result *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get info type (row[0]) */
      InfoType = Inf_ConvertFromStrDBToInfoType (row[0]);

      Gbl.CurrentCrs.Info.MustBeRead[InfoType] = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return (NumRows != 0);
  }

/*****************************************************************************/
/***** Write message if students must read any info about current course *****/
/*****************************************************************************/

void Inf_WriteMsgYouMustReadInfo (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_You_should_read_the_following_information_on_the_course_X;
   Inf_InfoType_t InfoType;

   /***** Write message *****/
   sprintf (Gbl.Message,Txt_You_should_read_the_following_information_on_the_course_X,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Write every information I must read *****/
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto;\">"
	              "<tr>"
	              "<td style=\"text-align:left;\">"
	              "<ul>");
   for (InfoType = (Inf_InfoType_t) 0;
	InfoType < Inf_NUM_INFO_TYPES;
	InfoType++)
      if (Gbl.CurrentCrs.Info.MustBeRead[InfoType])
        {
         fprintf (Gbl.F.Out,"<li>");
         Act_FormStart (Inf_ActionsSeeInfo[InfoType]);
         Act_LinkFormSubmit (Act_GetTitleAction (Inf_ActionsSeeInfo[InfoType]),The_ClassFormul[Gbl.Prefs.Theme]);
         fprintf (Gbl.F.Out,"%s"
                            "</a>",
                  Act_GetTitleAction (Inf_ActionsSeeInfo[InfoType]));
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</li>");
        }
   fprintf (Gbl.F.Out,"</ul>"
	              "</td>"
	              "</tr>"
	              "</table>");
  }

/*****************************************************************************/
/****** Change teacher's preference about force students to read info ********/
/*****************************************************************************/

void Inf_ChangeForceReadInfo (void)
  {
   extern const char *Txt_Students_now_are_required_to_read_this_information;
   extern const char *Txt_Students_are_no_longer_obliged_to_read_this_information;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   bool MustBeRead = Inf_GetMustBeReadFromForm ();

   /***** Set status (if info must be read or not) into database *****/
   Inf_SetForceReadIntoDB (InfoType,MustBeRead);

   /***** Write message of success *****/
   Lay_ShowAlert (Lay_SUCCESS,
                  MustBeRead ? Txt_Students_now_are_required_to_read_this_information :
                               Txt_Students_are_no_longer_obliged_to_read_this_information);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************** Change confirmation of I have read course info ***************/
/*****************************************************************************/

void Inf_ChangeIHaveReadInfo (void)
  {
   extern const char *Txt_You_have_confirmed_that_you_have_read_this_information;
   extern const char *Txt_You_have_eliminated_the_confirmation_that_you_have_read_this_information;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   bool IHaveRead = Inf_GetIfIHaveReadFromForm ();

   /***** Set status (if I have read or not a information) into database *****/
   Inf_SetIHaveReadIntoDB (InfoType,IHaveRead);

   /***** Write message of success *****/
   Lay_ShowAlert (Lay_SUCCESS,
                  IHaveRead ? Txt_You_have_confirmed_that_you_have_read_this_information :
                              Txt_You_have_eliminated_the_confirmation_that_you_have_read_this_information);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************* Get if info must be read by students from form ****************/
/*****************************************************************************/

static bool Inf_GetMustBeReadFromForm (void)
  {
   char YN[1+1];

   /***** Get a parameter that indicates if info must be read by students ******/
   Par_GetParToText ("MustBeRead",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/************* Get if info must be read by students from form ****************/
/*****************************************************************************/

static bool Inf_GetIfIHaveReadFromForm (void)
  {
   char YN[1+1];

   /***** Get a parameter that indicates if I have read a course info ******/
   Par_GetParToText ("IHaveRead",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/***************** Set if students must read course info *********************/
/*****************************************************************************/

static void Inf_SetForceReadIntoDB (Inf_InfoType_t InfoType,bool MustBeRead)
  {
   char Query[512];

   /***** Insert or replace info source for a specific type of course information *****/
   sprintf (Query,"UPDATE crs_info_src SET MustBeRead='%c'"
                  " WHERE CrsCod='%ld' AND InfoType='%s'",
            MustBeRead ? 'Y' :
        	         'N',
            Gbl.CurrentCrs.Crs.CrsCod,Inf_NamesInDBForInfoType[InfoType]);
   DB_QueryUPDATE (Query,"can not update if info must be read");
  }

/*****************************************************************************/
/********************* Set if I have read course info ************************/
/*****************************************************************************/

static void Inf_SetIHaveReadIntoDB (Inf_InfoType_t InfoType,bool IHaveRead)
  {
   char Query[512];

   if (IHaveRead)
     {
      /***** Insert I have read course information *****/
      sprintf (Query,"REPLACE INTO crs_info_read (UsrCod,CrsCod,InfoType)"
                     " VALUES ('%ld','%ld','%s')",
               Gbl.Usrs.Me.UsrDat.UsrCod,
               Gbl.CurrentCrs.Crs.CrsCod,
               Inf_NamesInDBForInfoType[InfoType]);
      DB_QueryUPDATE (Query,"can not set that I have read course info");
     }
   else
     {
      /***** Remove I have read course information *****/
      sprintf (Query,"DELETE FROM crs_info_read"
                     " WHERE UsrCod='%ld' AND CrsCod='%ld' AND InfoType='%s'",
               Gbl.Usrs.Me.UsrDat.UsrCod,
               Gbl.CurrentCrs.Crs.CrsCod,
               Inf_NamesInDBForInfoType[InfoType]);
      DB_QueryDELETE (Query,"can not set that I have not read course info");
     }
  }

/*****************************************************************************/
/********* Remove user's status about reading of course information **********/
/*****************************************************************************/

void Inf_RemoveUsrFromCrsInfoRead (long UsrCod,long CrsCod)
  {
   char Query[512];

   /***** Remove user's status about reading of course information *****/
   sprintf (Query,"DELETE FROM crs_info_read"
                  " WHERE UsrCod='%ld' AND CrsCod='%ld'",
            UsrCod,CrsCod);
   DB_QueryDELETE (Query,"can not set that I have not read course info");
  }

/*****************************************************************************/
/**************** Check if exists and show link to a page ********************/
/*****************************************************************************/

static void Inf_CheckAndShowPage (Inf_InfoType_t InfoType)
  {
   extern const char *Txt_No_information_available;
   const char *FileNameHTML;
   char PathRelFileHTML[PATH_MAX+1];
   char URL[PATH_MAX+1];

   // TODO !!!!!!!!!!!! If the page is hosted in server ==> it should be created a temporary public directory
   //                                                       and host the page in a private directory !!!!!!!!!!!!!!!!!

   /***** Open file with web page *****/
   /* 1. Check if index.html exists */
   FileNameHTML = "index.html";
   sprintf (PathRelFileHTML,"%s/%s",
	    Gbl.CurrentCrs.Info.Links[InfoType].PathRelWebPage,FileNameHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML))
     {
      sprintf (URL,"%s/%s",
	       Gbl.CurrentCrs.Info.Links[InfoType].URLWebPage,FileNameHTML);
      Inf_ShowPage (InfoType,URL);
     }
   else
     {
      /* 2. If index.html not exists, try index.htm */
      FileNameHTML = "index.htm";
      sprintf (PathRelFileHTML,"%s/%s",
	       Gbl.CurrentCrs.Info.Links[InfoType].PathRelWebPage,FileNameHTML);
      if (Fil_CheckIfPathExists (PathRelFileHTML))
	{
	 sprintf (URL,"%s/%s",
	          Gbl.CurrentCrs.Info.Links[InfoType].URLWebPage,FileNameHTML);
	 Inf_ShowPage (InfoType,URL);
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_No_information_available);
     }
  }

/*****************************************************************************/
/************* Check if exists and write page into HTML buffer ***************/
/*****************************************************************************/
// This function is called only from web service

int Inf_WritePageIntoHTMLBuffer (Inf_InfoType_t InfoType,char **HTMLBuffer)
  {
   char PathRelFileHTML[PATH_MAX+1];
   FILE *FileHTML;
   bool FileExists = false;
   size_t Length;

   /***** Initialize buffer *****/
   *HTMLBuffer = NULL;

   /***** Open file with web page *****/
   /* 1. Check if index.html exists */
   sprintf (PathRelFileHTML,"%s/index.html",
	    Gbl.CurrentCrs.Info.Links[InfoType].PathRelWebPage);
   if (Fil_CheckIfPathExists (PathRelFileHTML))
      FileExists = true;
   else
     {
      /* 2. If index.html not exists, try index.htm */
      sprintf (PathRelFileHTML,"%s/index.htm",
	       Gbl.CurrentCrs.Info.Links[InfoType].PathRelWebPage);
      if (Fil_CheckIfPathExists (PathRelFileHTML))
         FileExists = true;
     }

   if (FileExists)
     {
      /***** Write page from file to text buffer *****/
      /* Open file */
      if ((FileHTML = fopen (PathRelFileHTML,"rb")) == NULL)
	 Lay_ShowErrorAndExit ("Can not open XML file.");

      /* Compute file size */
      fseek (FileHTML,0L,SEEK_END);
      Length = (size_t) ftell (FileHTML);
      fseek (FileHTML,0L,SEEK_SET);

      /* Allocate memory for buffer */
      if ((*HTMLBuffer = (char *) malloc (Length+1)) == NULL)
	{
	 fclose (FileHTML);
	 Lay_ShowErrorAndExit ("Not enough memory for buffer.");
         return soap_receiver_fault (Gbl.soap,
                                     "Web page can not be copied into buffer",
                                     "Not enough memory for buffer");
	}

      /* Copy file content into buffer */
      if (fread ((void *) *HTMLBuffer,sizeof (char),Length,FileHTML) != Length)
	{
	 fclose (FileHTML);
         return soap_receiver_fault (Gbl.soap,
                                     "Web page can not be copied into buffer",
                                     "Error reading web page into buffer");
	}
      (*HTMLBuffer)[Length] = '\0';

      /***** Close HTML file *****/
      fclose (FileHTML);
     }

   return SOAP_OK;
  }

/*****************************************************************************/
/**************** Check if exists and show link to a page ********************/
/*****************************************************************************/

static void Inf_CheckAndShowURL (Inf_InfoType_t InfoType)
  {
   extern const char *Txt_No_information_available;
   FILE *FileURL;

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (Gbl.CurrentCrs.Info.Links[InfoType].PathRelFileURL ,"rb")))
     {
      if (fgets (Gbl.CurrentCrs.Info.URL,Cns_MAX_BYTES_URL,FileURL) == NULL)
	 Gbl.CurrentCrs.Info.URL[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);
      if (Gbl.CurrentCrs.Info.URL[0])
	 Inf_ShowPage (InfoType,Gbl.CurrentCrs.Info.URL);
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_No_information_available);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_No_information_available);
  }

/*****************************************************************************/
/************* Check if exists and write URL into text buffer ****************/
/*****************************************************************************/
// This function is called only from web service

void Inf_WriteURLIntoTxtBuffer (Inf_InfoType_t InfoType,char TxtBuffer[Cns_MAX_BYTES_URL+1])
  {
   FILE *FileURL;

   /***** Initialize buffer *****/
   TxtBuffer[0] = '\0';

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (Gbl.CurrentCrs.Info.Links[InfoType].PathRelFileURL,"rb")))
     {
      if (fgets (TxtBuffer,Cns_MAX_BYTES_URL,FileURL) == NULL)
	 TxtBuffer[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);
     }
  }

/*****************************************************************************/
/*************** Show link to a internal or external a page ******************/
/*****************************************************************************/

static void Inf_ShowPage (Inf_InfoType_t InfoType,const char *URL)
  {
   extern const char *The_ClassFormulB[The_NUM_THEMES];
   extern const char *Txt_View_in_a_new_window;
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];

   /***** Start of frame *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_INFO_TITLE[InfoType]);

   /***** Link to view in a new window *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td style=\"text-align:center;\">"
		      "<a href=\"%s\" target=\"_blank\" class=\"%s\">",
	    URL,The_ClassFormulB[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("fullscreen",
		    Txt_View_in_a_new_window,
		    Txt_View_in_a_new_window);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** End of frame *****/
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/************************** Set course info source ***************************/
/*****************************************************************************/

void Inf_SetInfoSrc (void)
  {
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   Inf_InfoSrc_t InfoSrcSelected = Inf_GetInfoSrcFromForm ();

   /***** Set info source into database *****/
   Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,InfoSrcSelected);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************** Select course info source and send course info ***************/
/*****************************************************************************/

void Inf_FormsToSelSendInfo (void)
  {
   extern const char *Txt_Source_of_information;
   extern const char *Txt_INFO_SRC_FULL_TEXT[Inf_NUM_INFO_SOURCES];
   extern const char *Txt_INFO_SRC_HELP[Inf_NUM_INFO_SOURCES];
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   Inf_InfoSrc_t InfoSrc,InfoSrcSelected;
   bool MustBeRead;

   /***** Get info source from database *****/
   Inf_GetInfoSrcFromDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,&InfoSrcSelected,&MustBeRead);

   /***** Forms for the different edition alternatives *****/
   /* Start of table */
   Lay_StartRoundFrameTable10 (NULL,0,NULL);

   /* Forms */
   for (InfoSrc = (Inf_InfoSrc_t) 1;
	InfoSrc < Inf_NUM_INFO_SOURCES;
	InfoSrc++)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"TIT\" style=\"text-align:center;");
      if (InfoSrc == InfoSrcSelected)
         fprintf (Gbl.F.Out," background-color:%s;",VERY_LIGHT_BLUE);
      fprintf (Gbl.F.Out,"\">"
	                 "&nbsp;<br />%s",
               Txt_INFO_SRC_FULL_TEXT[InfoSrc]);
      if (Txt_INFO_SRC_HELP[InfoSrc])
         fprintf (Gbl.F.Out,"<br />(%s)",
                  Txt_INFO_SRC_HELP[InfoSrc]);
      Inf_FormsForEditionTypes[InfoSrc] (InfoSrc,InfoType);
      fprintf (Gbl.F.Out,"<br />"
	                 "</td>"
	                 "</tr>");
     }

   /* End of table */
   Lay_EndRoundFrameTable10 ();

   /***** Form to choice between alternatives *****/
   /* Start of form and table */
   Act_FormStart (Inf_ActionsSelecInfoSrc[InfoType]);
   Lay_StartRoundFrameTable10 (NULL,0,NULL);

   /* Title */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT\" style=\"text-align:center;\">"
	              "%s"
	              "</td>"
	              "</tr>",
            Txt_Source_of_information);

   /* Options */
   for (InfoSrc = (Inf_InfoSrc_t) 0;
	InfoSrc < Inf_NUM_INFO_SOURCES;
	InfoSrc++)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;");
      if (InfoSrc == InfoSrcSelected)
         fprintf (Gbl.F.Out," background-color:%s;",VERY_LIGHT_BLUE);
      fprintf (Gbl.F.Out,"\">"
	                 "<input type=\"radio\" name=\"InfoSrc\" value=\"%u\"",
	       (unsigned) InfoSrc);
      if (InfoSrc == InfoSrcSelected)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"javascript:document.getElementById('%s').submit();\" />"
	                 "%s"
	                 "</td>"
	                 "</tr>",
               Gbl.FormId,Txt_INFO_SRC_FULL_TEXT[InfoSrc]);
     }

   /* End of table and form */
   Lay_EndRoundFrameTable10 ();
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Form to enter in integrated editor ***********************/
/*****************************************************************************/

void Inf_FormToEnterIntegratedEditor (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType)
  {
   extern const char *Txt_Edit;

   /***** Start form *****/
   Act_FormStart (Inf_ActionsInfo[InfoSrc][InfoType]);

   /***** Send button *****/
   Lay_PutConfirmButton (Txt_Edit);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Form to enter in plain text editor ***********************/
/*****************************************************************************/

void Inf_FormToEnterPlainTextEditor (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType)
  {
   extern const char *Txt_Edit_text;

   /***** Start form *****/
   Act_FormStart (Inf_ActionsInfo[InfoSrc][InfoType]);

   /***** Send button *****/
   Lay_PutConfirmButton (Txt_Edit_text);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Form to enter in rich text editor ***********************/
/*****************************************************************************/

void Inf_FormToEnterRichTextEditor (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType)
  {
   extern const char *Txt_Edit_text;

   /***** Start form *****/
   Act_FormStart (Inf_ActionsInfo[InfoSrc][InfoType]);

   /***** Send button *****/
   Lay_PutConfirmButton (Txt_Edit_text);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Form to upload a file with a page ***********************/
/*****************************************************************************/

void Inf_FormToSendPage (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_File;
   extern const char *Txt_Upload_file;

   /***** Start form *****/
   Act_FormStart (Inf_ActionsInfo[InfoSrc][InfoType]);

   /***** File *****/
   fprintf (Gbl.F.Out,"<table style=\"margin-left:auto; margin-right:auto;\">"
	              "<tr>"
                      "<td class=\"%s\" style=\"text-align:right;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<input type=\"file\" name=\"%s\" size=\"50\" maxlength=\"100\" value=\"\" />"
                      "</td>"
                      "</tr>"
                      "</table>",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_File,
            Fil_NAME_OF_PARAM_FILENAME_ORG);

   /***** Send button *****/
   Lay_PutCreateButton (Txt_Upload_file);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/********************* Form to send a link to a web page *********************/
/*****************************************************************************/

void Inf_FormToSendURL (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_URL;
   extern const char *Txt_Send_URL;
   FILE *FileURL;

   /***** Start form *****/
   Act_FormStart (Inf_ActionsInfo[InfoSrc][InfoType]);

   /***** Link *****/
   fprintf (Gbl.F.Out,"<table style=\"margin-left:auto; margin-right:auto;\">"
                      "<tr>"
                      "<td class=\"%s\" style=\"text-align:right;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<input type=\"text\" name=\"InfoSrcURL\" size=\"50\" maxlength=\"256\" value=\"",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_URL);
   if ((FileURL = fopen (Gbl.CurrentCrs.Info.Links[InfoType].PathRelFileURL,"rb")) == NULL)
      fprintf (Gbl.F.Out,"http://");
   else
     {
      if (fgets (Gbl.CurrentCrs.Info.URL,Cns_MAX_BYTES_URL,FileURL) == NULL)
         Gbl.CurrentCrs.Info.URL[0] = '\0';
      /* File is not needed now. Close it */
      fclose (FileURL);
      fprintf (Gbl.F.Out,"%s",Gbl.CurrentCrs.Info.URL);
     }
   fprintf (Gbl.F.Out,"\" />"
                      "</td>"
	              "</tr>"
	              "</table>");

   /***** Send button *****/
   Lay_PutCreateButton (Txt_Send_URL);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******** Returns bibliography, assessment, etc. from Gbl.CurrentAct *********/
/*****************************************************************************/

Inf_InfoType_t Inf_AsignInfoType (void)
  {
   switch (Gbl.CurrentAct)
     {
      case ActSeeCrsInf:
      case ActEdiCrsInf:
      case ActChgFrcReaCrsInf:
      case ActChgHavReaCrsInf:
      case ActSelInfSrcCrsInf:
      case ActEditorCrsInf:
      case ActRcvURLCrsInf:
      case ActRcvPagCrsInf:
      case ActPlaTxtEdiCrsInf:
      case ActRchTxtEdiCrsInf:
      case ActRcvPlaTxtCrsInf:
      case ActRcvRchTxtCrsInf:
         return Inf_INTRODUCTION;
      case ActSeeTchGui:
      case ActEdiTchGui:
      case ActChgFrcReaTchGui:
      case ActChgHavReaTchGui:
      case ActSelInfSrcTchGui:
      case ActEditorTchGui:
      case ActRcvURLTchGui:
      case ActRcvPagTchGui:
      case ActPlaTxtEdiTchGui:
      case ActRchTxtEdiTchGui:
      case ActRcvPlaTxtTchGui:
      case ActRcvRchTxtTchGui:
         return Inf_TEACHING_GUIDE;
      case ActSeeSyl:
	 Syl_GetParamWhichSyllabus ();
	 return (Gbl.CurrentCrs.Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
	                                                                 Inf_PRACTICALS);
      case ActSeeSylLec:
      case ActEdiSylLec:
      case ActChgFrcReaSylLec:
      case ActChgHavReaSylLec:
      case ActSelInfSrcSylLec:
      case ActEditorSylLec:
      case ActRcvURLSylLec:
      case ActRcvPagSylLec:
      case ActPlaTxtEdiSylLec:
      case ActRchTxtEdiSylLec:
      case ActRcvPlaTxtSylLec:
      case ActRcvRchTxtSylLec:
	 Gbl.CurrentCrs.Syllabus.WhichSyllabus = Syl_LECTURES;
         return Inf_LECTURES;
      case ActSeeSylPra:
      case ActEdiSylPra:
      case ActChgFrcReaSylPra:
      case ActChgHavReaSylPra:
      case ActSelInfSrcSylPra:
      case ActEditorSylPra:
      case ActRcvURLSylPra:
      case ActRcvPagSylPra:
      case ActPlaTxtEdiSylPra:
      case ActRchTxtEdiSylPra:
      case ActRcvPlaTxtSylPra:
      case ActRcvRchTxtSylPra:
	 Gbl.CurrentCrs.Syllabus.WhichSyllabus = Syl_PRACTICALS;
         return Inf_PRACTICALS;
      case ActSeeBib:
      case ActEdiBib:
      case ActChgFrcReaBib:
      case ActChgHavReaBib:
      case ActSelInfSrcBib:
      case ActEditorBib:
      case ActRcvURLBib:
      case ActRcvPagBib:
      case ActPlaTxtEdiBib:
      case ActRchTxtEdiBib:
      case ActRcvPlaTxtBib:
      case ActRcvRchTxtBib:
         return Inf_BIBLIOGRAPHY;
      case ActSeeFAQ:
      case ActEdiFAQ:
      case ActChgFrcReaFAQ:
      case ActChgHavReaFAQ:
      case ActSelInfSrcFAQ:
      case ActEditorFAQ:
      case ActRcvURLFAQ:
      case ActRcvPagFAQ:
      case ActPlaTxtEdiFAQ:
      case ActRchTxtEdiFAQ:
      case ActRcvPlaTxtFAQ:
      case ActRcvRchTxtFAQ:
         return Inf_FAQ;
      case ActSeeCrsLnk:
      case ActEdiCrsLnk:
      case ActChgFrcReaCrsLnk:
      case ActChgHavReaCrsLnk:
      case ActSelInfSrcCrsLnk:
      case ActEditorCrsLnk:
      case ActRcvURLCrsLnk:
      case ActRcvPagCrsLnk:
      case ActPlaTxtEdiCrsLnk:
      case ActRchTxtEdiCrsLnk:
      case ActRcvPlaTxtCrsLnk:
      case ActRcvRchTxtCrsLnk:
         return Inf_LINKS;
      case ActSeeAss:
      case ActEdiAss:
      case ActChgFrcReaAss:
      case ActChgHavReaAss:
      case ActSelInfSrcAss:
      case ActEditorAss:
      case ActRcvURLAss:
      case ActRcvPagAss:
      case ActPlaTxtEdiAss:
      case ActRchTxtEdiAss:
      case ActRcvPlaTxtAss:
      case ActRcvRchTxtAss:
         return Inf_ASSESSMENT;
     }
   return (Inf_InfoType_t) 0; // If action is not one of the former ones, the returned value is not important
  }

/*****************************************************************************/
/********** Get info source for bibliography, FAQ, etc. from form ************/
/*****************************************************************************/

Inf_InfoSrc_t Inf_GetInfoSrcFromForm (void)
  {
   char UnsignedStr[10+1];
   int InfoSrcInt;
   Inf_InfoSrc_t InfoSrc;

   /***** Get info source for a specific type of course information
          (introduction, teaching guide, bibliography, FAQ, links or evaluation) *****/
   Par_GetParToText ("InfoSrc",UnsignedStr,2);
   if (sscanf (UnsignedStr,"%d",&InfoSrcInt) != 1)
      return Inf_INFO_SRC_NONE;
   InfoSrc = (Inf_InfoSrc_t) InfoSrcInt;
   return InfoSrc;
  }

/*****************************************************************************/
/********* Set info source for a type of course info from database ***********/
/*****************************************************************************/

void Inf_SetInfoSrcIntoDB (long CrsCod,Inf_InfoType_t InfoType,Inf_InfoSrc_t InfoSrc)
  {
   char Query[512];

   /***** Get if info source is already stored in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_info_src"
                  " WHERE CrsCod='%ld' AND InfoType='%s'",
            CrsCod,Inf_NamesInDBForInfoType[InfoType]);
   if (DB_QueryCOUNT (Query,"can not get if info source is already stored in database"))	// Info is already stored in database, so update it
     {	// Update info source
      if (InfoSrc == Inf_INFO_SRC_NONE)
         sprintf (Query,"UPDATE crs_info_src SET InfoSrc='%s',MustBeRead='N'"
                        " WHERE CrsCod='%ld' AND InfoType='%s'",
                  Inf_NamesInDBForInfoSrc[Inf_INFO_SRC_NONE],
                  CrsCod,Inf_NamesInDBForInfoType[InfoType]);
      else	// MustBeRead remains unchanged
         sprintf (Query,"UPDATE crs_info_src SET InfoSrc='%s'"
                        " WHERE CrsCod='%ld' AND InfoType='%s'",
                  Inf_NamesInDBForInfoSrc[InfoSrc],
                  CrsCod,Inf_NamesInDBForInfoType[InfoType]);
      DB_QueryUPDATE (Query,"can not update info source");
     }
   else		// Info is not stored in database, so insert it
     {
      sprintf (Query,"INSERT INTO crs_info_src (CrsCod,InfoType,InfoSrc,MustBeRead)"
                     " VALUES ('%ld','%s','%s','N')",
               CrsCod,Inf_NamesInDBForInfoType[InfoType],Inf_NamesInDBForInfoSrc[InfoSrc]);
      DB_QueryINSERT (Query,"can not insert info source");
     }
  }

/*****************************************************************************/
/********* Get info source for a type of course info from database ***********/
/*****************************************************************************/

void Inf_GetInfoSrcFromDB (long CrsCod,Inf_InfoType_t InfoType,Inf_InfoSrc_t *InfoSrc,bool *MustBeRead)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   char PathFile[PATH_MAX+1];
   FILE *File;
   char URL[Cns_MAX_BYTES_URL+1];

   /***** Set default values *****/
   *InfoSrc = Inf_INFO_SRC_NONE;
   *MustBeRead = false;

   /***** Get info source for a specific type of course information
          (bibliography, FAQ, links or evaluation) from database *****/
   sprintf (Query,"SELECT InfoSrc,MustBeRead FROM crs_info_src"
	          " WHERE CrsCod='%ld' AND InfoType='%s'",
            CrsCod,Inf_NamesInDBForInfoType[InfoType]);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get info source");

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get info source (row[0]) */
      *InfoSrc = Inf_ConvertFromStrDBToInfoSrc (row[0]);

      /* Get if students must read info (row[1]) */
      *MustBeRead = (Str_ConvertToUpperLetter (row[1][0]) == 'Y');;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting info source.");

   /***** If info is empty, return Inf_INFO_SRC_NONE *****/
   switch (*InfoSrc)
     {
      case Inf_INFO_SRC_NONE:
         *MustBeRead = false;
         break;
      case Inf_INFO_SRC_EDITOR:
         switch (InfoType)
           {
            case Inf_LECTURES:
            case Inf_PRACTICALS:
               /***** Check if file exists *****/
               sprintf (PathFile,"%s/%s/%ld/%s/%s",
                        Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,CrsCod,
                        InfoType == Inf_LECTURES ? Cfg_SYLLABUS_FOLDER_LECTURES :
                        	                   Cfg_SYLLABUS_FOLDER_PRACTICALS,
                        Cfg_SYLLABUS_FILENAME);
               if (!Fil_CheckIfPathExists (PathFile))	// Should be XML file checked for items!!!!
                 {
                  *InfoSrc = Inf_INFO_SRC_NONE;
                  *MustBeRead = false;
                 }
               break;
            case Inf_INTRODUCTION:
            case Inf_TEACHING_GUIDE:
            case Inf_BIBLIOGRAPHY:
            case Inf_FAQ:
            case Inf_LINKS:
            case Inf_ASSESSMENT:
               *InfoSrc = Inf_INFO_SRC_NONE;
               *MustBeRead = false;
	       break;	// Internal editor is not yet available
           }
         break;
      case Inf_INFO_SRC_PLAIN_TEXT:
         if (!Inf_CheckIfInfoTxtIsNotEmpty (CrsCod,InfoType))
           {
            *InfoSrc = Inf_INFO_SRC_NONE;
            *MustBeRead = false;
           }
         break;
      case Inf_INFO_SRC_RICH_TEXT:
         if (!Inf_CheckIfInfoTxtIsNotEmpty (CrsCod,InfoType))
           {
            *InfoSrc = Inf_INFO_SRC_NONE;
            *MustBeRead = false;
           }
         break;
      case Inf_INFO_SRC_PAGE:
         /***** Check if file with web page exists *****/
         sprintf (PathFile,"%s/%s/%ld/%s/index.html",
                  Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,CrsCod,Inf_FileNamesForInfoType[InfoType]);
         if (!Fil_CheckIfPathExists (PathFile))
           {
            // If index.html not exists, try index.htm
            sprintf (PathFile,"%s/%s/%ld/%s/index.htm",
                     Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CRS,CrsCod,Inf_FileNamesForInfoType[InfoType]);
            if (!Fil_CheckIfPathExists (PathFile))
              {
               *InfoSrc = Inf_INFO_SRC_NONE;
               *MustBeRead = false;
              }
           }
         break;
      case Inf_INFO_SRC_URL:
         /***** Check if URL in file is empty or not *****/
         sprintf (PathFile,"%s/%s/%ld/%s.url",
                  Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,CrsCod,Inf_FileNamesForInfoType[InfoType]);
         File = fopen (PathFile,"rb");
         if (File)
           {
            if (fgets (URL,Cns_MAX_BYTES_URL,File) == NULL)
            	 URL[0] = '\0';
            /* File is not longer needed  ==> close it */
            fclose (File);
            if (!URL[0])
              {
               *InfoSrc = Inf_INFO_SRC_NONE;
               *MustBeRead = false;
              }
           }
         else
           {
            *InfoSrc = Inf_INFO_SRC_NONE;
            *MustBeRead = false;
           }
         break;
     }
  }

/*****************************************************************************/
/*** Convert a string with info type in database to a Inf_InfoType_t value ***/
/*****************************************************************************/

Inf_InfoType_t Inf_ConvertFromStrDBToInfoType (const char *StrInfoTypeDB)
  {
   Inf_InfoType_t InfoType;

   for (InfoType = (Inf_InfoType_t) 0;
	InfoType < Inf_NUM_INFO_TYPES;
	InfoType++)
      if (!strcmp (StrInfoTypeDB,Inf_NamesInDBForInfoType[InfoType]))
         return InfoType;

   return (Inf_InfoType_t) 0;
  }

/*****************************************************************************/
/** Convert a string with info source in database to a Inf_InfoSrc_t value ***/
/*****************************************************************************/

Inf_InfoSrc_t Inf_ConvertFromStrDBToInfoSrc (const char *StrInfoSrcDB)
  {
   Inf_InfoSrc_t InfoSrc;

   for (InfoSrc = (Inf_InfoSrc_t) 0;
	InfoSrc < Inf_NUM_INFO_SOURCES;
	InfoSrc++)
      if (!strcmp (StrInfoSrcDB,Inf_NamesInDBForInfoSrc[InfoSrc]))
         return InfoSrc;

   return (Inf_InfoSrc_t) 0;
  }

/*****************************************************************************/
/********** Set info text for a type of course info from database ************/
/*****************************************************************************/

void Inf_SetInfoTxtIntoDB (long CrsCod,Inf_InfoType_t InfoType,const char *InfoTxt)
  {
   char Query[256+Cns_MAX_BYTES_LONG_TEXT];

   /***** Insert or replace info source for a specific type of course information *****/
   sprintf (Query,"REPLACE INTO crs_info_txt (CrsCod,InfoType,InfoTxt)"
                  " VALUES ('%ld','%s','%s')",
            CrsCod,Inf_NamesInDBForInfoType[InfoType],InfoTxt);
   DB_QueryREPLACE (Query,"can not update info text");
  }

/*****************************************************************************/
/********** Get info text for a type of course info from database ************/
/*****************************************************************************/

void Inf_GetInfoTxtFromDB (Inf_InfoType_t InfoType,char *InfoTxt,size_t MaxLength)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get info source for a specific type of course information
          (bibliography, FAQ, links or evaluation) from database *****/
   sprintf (Query,"SELECT InfoTxt FROM crs_info_txt"
                  " WHERE CrsCod='%ld' AND InfoType='%s'",
            Gbl.CurrentCrs.Crs.CrsCod,Inf_NamesInDBForInfoType[InfoType]);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get info text");

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      strncpy (InfoTxt,row[0],MaxLength);
      InfoTxt[MaxLength] = '\0';
     }
   else
      InfoTxt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting info text.");
  }

/*****************************************************************************/
/*** Check if info text for a type of course info is not empty in database ***/
/*****************************************************************************/

bool Inf_CheckIfInfoTxtIsNotEmpty (long CrsCod,Inf_InfoType_t InfoType)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool Result;

   /***** Get info source for a specific type of course information
          (bibliography, FAQ, links or evaluation) from database *****/
   sprintf (Query,"SELECT InfoTxt FROM crs_info_txt"
                  " WHERE CrsCod='%ld' AND InfoType='%s'",
            CrsCod,Inf_NamesInDBForInfoType[InfoType]);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get info text");

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      Result = (row[0][0] != '\0');	// Is text empty?
     }
   else
      Result = false;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Result;
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/

static void Inf_ShowTxtInfo (Inf_InfoType_t InfoType)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   extern const char *Txt_No_information_available;
   char Txt[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (InfoType,Txt,Cns_MAX_BYTES_LONG_TEXT);

   if (Txt[0])
     {
      /***** Start table *****/
      Lay_StartRoundFrameTable10 (NULL,0,Txt_INFO_TITLE[InfoType]);

      if (InfoType == Inf_INTRODUCTION ||
          InfoType == Inf_TEACHING_GUIDE)
         Lay_WriteHeaderClassPhoto (3,false,false,Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentCrs.Crs.CrsCod);
      fprintf (Gbl.F.Out,"<tr>"
                         "<td style=\"text-align:left;\">"
                         "<p class=\"DAT\" style=\"text-align:justify;\">");

      /***** Convert to respectful HTML and insert links *****/
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Txt,Cns_MAX_BYTES_LONG_TEXT,false);	// Convert from HTML to recpectful HTML
      Str_InsertLinkInURLs (Txt,Cns_MAX_BYTES_LONG_TEXT,60);	// Insert links

      /***** Write text *****/
      fprintf (Gbl.F.Out,"%s",Txt);

      /***** Finish table *****/
      fprintf (Gbl.F.Out,"</p>"
	                 "</td>"
	                 "</tr>");
      Lay_EndRoundFrameTable10 ();
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_No_information_available);
  }

/*****************************************************************************/
/************* Check if exists and write page into HTML buffer ***************/
/*****************************************************************************/
// This function is called only from web service

int Inf_WritePlainTextIntoHTMLBuffer (Inf_InfoType_t InfoType,char **HTMLBuffer)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   char Txt[Cns_MAX_BYTES_LONG_TEXT+1];
   char FileNameHTMLTmp[PATH_MAX+1];
   FILE *FileHTMLTmp;
   size_t Length;

   /***** Initialize buffer *****/
   *HTMLBuffer = NULL;

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (InfoType,Txt,Cns_MAX_BYTES_LONG_TEXT);

   if (Txt[0])
     {
      /***** Create a unique name for the file *****/
      sprintf (FileNameHTMLTmp,"%s/%s/%s_info.html",
	       Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_OUT,Gbl.UniqueNameEncrypted);

      /***** Create a new temporary file for writing and reading *****/
      if ((FileHTMLTmp = fopen (FileNameHTMLTmp,"w+b")) == NULL)
         return soap_receiver_fault (Gbl.soap,
                                     "Plain text can not be copied into buffer",
                                     "Can not create temporary file");

      /***** Write start of HTML code *****/
      fprintf (FileHTMLTmp,"<!DOCTYPE html>\n"
			    "<html lang=\"%s\">\n"
			    "<head>\n"
			    "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=windows-1252\" />\n"
			    "<title>%s</title>\n"
			    "</head>\n"
			    "<body>\n",
	       Txt_STR_LANG_ID[Gbl.Prefs.Language],	// Language
	       Txt_INFO_TITLE[InfoType]);		// Page title

      /***** Write plain text into text buffer *****/
      fprintf (FileHTMLTmp,"<tr>"
		            "<td style=\"text-align:left;\">"
		            "<p class=\"DAT\" style=\"text-align:justify;\">");

      /* Convert to respectful HTML and insert links */
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Txt,Cns_MAX_BYTES_LONG_TEXT,false);	// Convert from HTML to recpectful HTML
      Str_InsertLinkInURLs (Txt,Cns_MAX_BYTES_LONG_TEXT,60);	// Insert links

      /* Write text */
      fprintf (FileHTMLTmp,"%s",Txt);

      /***** Write end of page into file *****/
      fprintf (FileHTMLTmp,"</p>\n"
			    "</html>\n"
			    "</body>\n");

      /***** Compute length of file *****/
      Length = (size_t) ftell (FileHTMLTmp);

      /***** Allocate memory for buffer *****/
      if ((*HTMLBuffer = (char *) malloc (Length+1)) == NULL)
	{
	 fclose (FileHTMLTmp);
	 unlink (FileNameHTMLTmp);
         return soap_receiver_fault (Gbl.soap,
                                     "Plain text can not be copied into buffer",
                                     "Not enough memory for buffer");
	}

      /***** Copy file content into buffer *****/
      fseek (FileHTMLTmp,0L,SEEK_SET);
      if (fread ((void *) *HTMLBuffer,sizeof (char),Length,FileHTMLTmp) != Length)
	{
	 fclose (FileHTMLTmp);
	 unlink (FileNameHTMLTmp);
         return soap_receiver_fault (Gbl.soap,
                                     "Plain text can not be copied into buffer",
                                     "Error reading file into buffer");
	}
      (*HTMLBuffer)[Length] = '\0';

      /***** Close and remove temporary file *****/
      fclose (FileHTMLTmp);
      unlink (FileNameHTMLTmp);
     }

   return SOAP_OK;
  }

/*****************************************************************************/
/************** Edit plain text information about the course *****************/
/*****************************************************************************/

void Inf_EditPlainTxtInfo (void)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   extern const char *Txt_Send;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   char Txt[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Start table *****/
   Act_FormStart (Inf_ActionsRcvPlaTxtInfo[InfoType]);
   Lay_StartRoundFrameTable10 (NULL,0,Txt_INFO_TITLE[InfoType]);

   if (InfoType == Inf_INTRODUCTION ||
       InfoType == Inf_TEACHING_GUIDE)
      Lay_WriteHeaderClassPhoto (1,false,false,Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (InfoType,Txt,Cns_MAX_BYTES_LONG_TEXT);

   /***** Edition area *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td style=\"text-align:center;\">"
	              "<textarea name=\"Txt\" cols=\"80\" rows=\"20\">"
	              "%s"
	              "</textarea>"
	              "</td>"
	              "</tr>",
            Txt);

   /***** End of table *****/
   Lay_EndRoundFrameTable10 ();
   // fprintf (Gbl.F.Out,"<br />");

   /***** Send and undo buttons *****/
   Lay_PutCreateButton (Txt_Send);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************** Edit rich text information about the course *****************/
/*****************************************************************************/

void Inf_EditRichTxtInfo (void)
  {
   extern const char *Txt_The_rich_text_editor_is_not_yet_available;
   extern const char *Txt_Send;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   char Txt[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Under test... *****/
   /*
+--------+----------+---------------------+
| UsrCod | UsrID    | CreatTime           |
+--------+----------+---------------------+
|   1346 | 24243619 | 2013-11-25 00:29:13 |
|  24383 | 53590723 | 2013-11-25 00:29:13 |
+--------+----------+---------------------+
    */
   if (Gbl.Usrs.Me.UsrDat.UsrCod != 1346 &&
       Gbl.Usrs.Me.UsrDat.UsrCod != 24383)	// TODO: Remove this when rich text editor is available
     {
      Lay_ShowAlert (Lay_WARNING,Txt_The_rich_text_editor_is_not_yet_available);

      /***** Show again the form to select and send course info *****/
      Inf_FormsToSelSendInfo ();
      return;
     }

   /***** Start table *****/
   Act_FormStart (Inf_ActionsRcvRchTxtInfo[InfoType]);
   Lay_StartRoundFrameTable10 (NULL,0,NULL);

   if (InfoType == Inf_INTRODUCTION ||
       InfoType == Inf_TEACHING_GUIDE)
      Lay_WriteHeaderClassPhoto (1,false,false,Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (InfoType,Txt,Cns_MAX_BYTES_LONG_TEXT);

   /***** Edition area *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td style=\"text-align:center;\">"
	              "<textarea name=\"Txt\" cols=\"80\" rows=\"20\">"
	              "%s"
	              "</textarea>"
	              "</td>"
	              "</tr>",
            Txt);

   /***** End of table *****/
   Lay_EndRoundFrameTable10 ();
   // fprintf (Gbl.F.Out,"<br />");

   /***** Send and undo buttons *****/
   Lay_PutCreateButton (Txt_Send);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************** Receive and change plain text of course info ****************/
/*****************************************************************************/

void Inf_RecAndChangePlainTxtInfo (void)
  {
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   char Txt[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get text with course information from form *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_LONG_TEXT);	// Store in HTML format (not rigorous)

   /***** Update text of course info in database *****/
   Inf_SetInfoTxtIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,Txt);

   /***** Change info source to text in database *****/
   Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,
                         Txt[0] ? Inf_INFO_SRC_PLAIN_TEXT :
	                          Inf_INFO_SRC_NONE);
   if (Txt[0])
      /***** Show the updated info *****/
      Inf_ShowInfo ();
   else
      /***** Show again the form to select and send course info *****/
      Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/*************** Receive and change rich text of course info *****************/
/*****************************************************************************/

void Inf_RecAndChangeRichTxtInfo (void)
  {
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   char Txt[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get text with course information from form *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_LONG_TEXT);	// Store in HTML format (not rigorous)

   /***** Update text of course info in database *****/
   Inf_SetInfoTxtIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,Txt);

   /***** Change info source to text in database *****/
   Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,
                         Txt[0] ? Inf_INFO_SRC_RICH_TEXT :
	                          Inf_INFO_SRC_NONE);
   if (Txt[0])
      /***** Show the updated info *****/
      Inf_ShowInfo ();
   else
      /***** Show again the form to select and send course info *****/
      Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/********* Receive a link to a page of syllabus, bibliography, etc. **********/
/*****************************************************************************/

void Inf_ReceiveURLInfo (void)
  {
   extern const char *Txt_The_URL_X_has_been_updated;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   FILE *FileURL;
   bool URLIsOK = false;

   /***** Get parameter with URL *****/
   Par_GetParToText ("InfoSrcURL",Gbl.CurrentCrs.Info.URL,Cns_MAX_BYTES_URL);

   /***** Open file with URL *****/
   if ((FileURL = fopen (Gbl.CurrentCrs.Info.Links[InfoType].PathRelFileURL,"wb")) != NULL)
     {
      /***** Write URL *****/
      fprintf (FileURL,"%s",Gbl.CurrentCrs.Info.URL);

      /***** Close file *****/
      fclose (FileURL);

      /***** Write message *****/
      sprintf (Gbl.Message,Txt_The_URL_X_has_been_updated,
               Gbl.CurrentCrs.Info.URL);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
      URLIsOK = true;
     }
   else
      Lay_ShowAlert (Lay_ERROR,"Error when saving URL to file.");

   if (URLIsOK)
     {
      /***** Change info source to URL in database *****/
      Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,Inf_INFO_SRC_URL);

      /***** Show the updated info *****/
      Inf_ShowInfo ();
     }
   else
     {
      /***** Change info source to none in database *****/
      Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,Inf_INFO_SRC_NONE);

      /***** Show again the form to select and send course info *****/
      Inf_FormsToSelSendInfo ();
     }
  }

/*****************************************************************************/
/************** Receive a page of syllabus, bibliography, etc. ***************/
/*****************************************************************************/

void Inf_ReceivePagInfo (void)
  {
   extern const char *Txt_The_file_type_is_X_and_should_be_HTML_or_ZIP;
   extern const char *Txt_The_HTML_file_has_been_received_successfully;
   extern const char *Txt_The_ZIP_file_has_been_received_successfully;
   extern const char *Txt_The_ZIP_file_has_been_unzipped_successfully;
   extern const char *Txt_Found_an_index_html_file;
   extern const char *Txt_Found_an_index_htm_file;
   extern const char *Txt_No_file_index_html_index_htm_found_within_the_ZIP_file;
   extern const char *Txt_The_file_type_should_be_HTML_or_ZIP;
   Inf_InfoType_t InfoType = Inf_AsignInfoType ();
   char SourceFileName[PATH_MAX+1];
   char PathRelFileHTML[PATH_MAX+1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   char StrUnzip[100+PATH_MAX*2+1];
   char *PathWebPage;
   bool FileIsOK = false;

   /***** First of all, store in disk the file from stdin (really from Gbl.F.Tmp) *****/
   Fil_StartReceptionOfFile (SourceFileName,MIMEType);

   /***** Check that MIME type is HTML or ZIP *****/
   if (strcmp (MIMEType,"text/html") &&
       strcmp (MIMEType,"text/plain") &&
       strcmp (MIMEType,"application/x-zip-compressed") &&
       strcmp (MIMEType,"application/zip") &&
       strcmp (MIMEType,"application/octet-stream") &&
       strcmp (MIMEType,"application/x-download"))
     {
      sprintf (Gbl.Message,Txt_The_file_type_is_X_and_should_be_HTML_or_ZIP,
               MIMEType);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }
   else
     {
      /***** End the reception of the data *****/
      PathWebPage = Gbl.CurrentCrs.Info.Links[InfoType].PathRelWebPage;
      if (Str_FileIs (SourceFileName,"html") ||
          Str_FileIs (SourceFileName,"htm" )) // .html or .htm file
        {
         Brw_RemoveTree (PathWebPage);
         Fil_CreateDirIfNotExists (PathWebPage);
         sprintf (PathRelFileHTML,"%s/index.html",PathWebPage);
         if (Fil_EndReceptionOfFile (PathRelFileHTML))
           {
            Lay_ShowAlert (Lay_SUCCESS,Txt_The_HTML_file_has_been_received_successfully);
            FileIsOK = true;
           }
         else
            Lay_ShowAlert (Lay_WARNING,"Error uploading file.");
        }
      else if (Str_FileIs (SourceFileName,"zip")) // .zip file
        {
         Brw_RemoveTree (PathWebPage);
         Fil_CreateDirIfNotExists (PathWebPage);
         if (Fil_EndReceptionOfFile (Gbl.CurrentCrs.Info.Links[InfoType].PathRelFileZIP))
           {
            Lay_ShowAlert (Lay_SUCCESS,Txt_The_ZIP_file_has_been_received_successfully);

            /* Uncompress ZIP */
            sprintf (StrUnzip,"unzip -qq -o %s -d %s",
                     Gbl.CurrentCrs.Info.Links[InfoType].PathRelFileZIP,PathWebPage);
            if (system (StrUnzip) == 0)
              {
               /* Check if uploaded file is index.html or index.htm */
               sprintf (PathRelFileHTML,"%s/index.html",PathWebPage);
               if (Fil_CheckIfPathExists (PathRelFileHTML))
                 {
                  Lay_ShowAlert (Lay_SUCCESS,Txt_The_ZIP_file_has_been_unzipped_successfully);
                  Lay_ShowAlert (Lay_SUCCESS,Txt_Found_an_index_html_file);
                  FileIsOK = true;
                 }
	       else
	         {
	          sprintf (PathRelFileHTML,"%s/index.htm",PathWebPage);
	          if (Fil_CheckIfPathExists (PathRelFileHTML))
                    {
                     Lay_ShowAlert (Lay_SUCCESS,Txt_The_ZIP_file_has_been_unzipped_successfully);
                     Lay_ShowAlert (Lay_SUCCESS,Txt_Found_an_index_htm_file);
                     FileIsOK = true;
                    }
	          else
                     Lay_ShowAlert (Lay_WARNING,Txt_No_file_index_html_index_htm_found_within_the_ZIP_file);
	         }
	      }
            else
               Lay_ShowErrorAndExit ("Can not unzip file.");
           }
         else
            Lay_ShowAlert (Lay_WARNING,"Error uploading file.");
        }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_The_file_type_should_be_HTML_or_ZIP);
     }

   if (FileIsOK)
     {
      /***** Change info source to page in database *****/
      Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,Inf_INFO_SRC_PAGE);

      /***** Show the updated info *****/
      Inf_ShowInfo ();
     }
   else
     {
      /***** Change info source to none in database *****/
      Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,InfoType,Inf_INFO_SRC_NONE);

      /***** Show again the form to select and send course info *****/
      Inf_FormsToSelSendInfo ();
     }
  }

/*****************************************************************************/
/******************* Integrated editor for introduction **********************/
/*****************************************************************************/

void Inf_EditorCourseInfo (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Lay_ShowAlert (Lay_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/****************** Integrated editor for teaching guide *********************/
/*****************************************************************************/

void Inf_EditorTeachingGuide (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Lay_ShowAlert (Lay_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/******************* Integrated editor for bibliography **********************/
/*****************************************************************************/

void Inf_EditorBibliography (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Lay_ShowAlert (Lay_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/*********************** Integrated editor for FAQ ***************************/
/*****************************************************************************/

void Inf_EditorFAQ (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Lay_ShowAlert (Lay_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/********************* Integrated editor for links ***************************/
/*****************************************************************************/

void Inf_EditorLinks (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Lay_ShowAlert (Lay_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_FormsToSelSendInfo ();
  }

/*****************************************************************************/
/****************** Integrated editor for assessment system ******************/
/*****************************************************************************/

void Inf_EditorAssessment (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Lay_ShowAlert (Lay_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_FormsToSelSendInfo ();
  }
