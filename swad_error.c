// swad_layout.c: page layout

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_log.h"
#include "swad_statistic.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/********** Write error message and exit when not enough memory **************/
/*****************************************************************************/

void Err_NotEnoughMemoryExit (void)
  {
   Err_ShowErrorAndExit ("Not enough memory.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong action ****************/
/*****************************************************************************/

void Err_WrongActionExit (void)
  {
   Err_ShowErrorAndExit ("Wrong action.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong scope *****************/
/*****************************************************************************/

void Err_WrongScopeExit (void)
  {
   Err_ShowErrorAndExit ("Wrong scope.");
  }

/*****************************************************************************/
/********* Write error message and exit                              *********/
/********* when wrong country, institution, center, degree or course *********/
/*****************************************************************************/

void Err_WrongCountrExit (void)
  {
   Err_ShowErrorAndExit ("Wrong country.");
  }

void Err_WrongInstitExit (void)
  {
   Err_ShowErrorAndExit ("Wrong institution.");
  }

void Err_WrongCenterExit (void)
  {
   Err_ShowErrorAndExit ("Wrong center.");
  }

void Err_WrongDegTypExit (void)
  {
   Err_ShowErrorAndExit ("Wrong degree type.");
  }

void Err_WrongDegreeExit (void)
  {
   Err_ShowErrorAndExit ("Wrong degree.");
  }

void Err_WrongCourseExit (void)
  {
   Err_ShowErrorAndExit ("Wrong course.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong department **************/
/*****************************************************************************/

void Err_WrongDepartmentExit (void)
  {
   Err_ShowErrorAndExit ("Wrong department.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong place ****************/
/*****************************************************************************/

void Err_WrongPlaceExit (void)
  {
   Err_ShowErrorAndExit ("Wrong department.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong order *****************/
/*****************************************************************************/

void Err_WrongOrderExit (void)
  {
   Err_ShowErrorAndExit ("Wrong order.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong status ****************/
/*****************************************************************************/

void Err_WrongStatusExit (void)
  {
   Err_ShowErrorAndExit ("Wrong status.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong date ****************/
/*****************************************************************************/

void Err_WrongDateExit (void)
  {
   Err_ShowErrorAndExit ("Wrong date.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong plugin ***************/
/*****************************************************************************/

void Err_WrongPluginExit (void)
  {
   Err_ShowErrorAndExit ("Wrong plugin.");
  }

/*****************************************************************************/
/********** Write error message and exit when wrong items list/item **********/
/*****************************************************************************/

void Err_WrongItemsListExit (void)
  {
   Err_ShowErrorAndExit ("Wrong items list.");
  }

void Err_WrongItemExit (void)
  {
   Err_ShowErrorAndExit ("Wrong item.");
  }

/*****************************************************************************/
/********** Write error message and exit when wrong syllabus format **********/
/*****************************************************************************/

void Err_WrongSyllabusFormatExit (void)
  {
   Err_ShowErrorAndExit ("Wrong syllabus format.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong file browser ************/
/*****************************************************************************/

void Err_WrongFileBrowserExit (void)
  {
   Err_ShowErrorAndExit ("Wrong file browser.");
  }

/*****************************************************************************/
/******* Write error message and exit when file/folder does not exist ********/
/*****************************************************************************/

void Err_FileFolderNotFoundExit (void)
  {
   Err_ShowErrorAndExit ("File/folder not found.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong copy source *************/
/*****************************************************************************/

void Err_WrongCopySrcExit (void)
  {
   Err_ShowErrorAndExit ("The copy source does not exist.");
  }

/*****************************************************************************/
/********** Write error message and exit when wrong number of rows ***********/
/*****************************************************************************/

void Err_WrongNumberOfRowsExit (void)
  {
   Err_ShowErrorAndExit ("Wrong number of rows.");
  }

/*****************************************************************************/
/********* Write error message and exit when wrong group type/group **********/
/*****************************************************************************/

void Err_WrongGrpTypExit (void)
  {
   Err_ShowErrorAndExit ("Wrong group type.");
  }

void Err_WrongGroupExit (void)
  {
   Err_ShowErrorAndExit ("Wrong group.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong order *****************/
/*****************************************************************************/

void Err_WrongTypeOfViewExit (void)
  {
   Err_ShowErrorAndExit ("Wrong type of view.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong banner/link *************/
/*****************************************************************************/

void Err_WrongBannerExit (void)
  {
   Err_ShowErrorAndExit ("Wrong banner.");
  }

void Err_WrongLinkExit (void)
  {
   Err_ShowErrorAndExit ("Wrong link.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong holiday ***************/
/*****************************************************************************/

void Err_WrongHolidayExit (void)
  {
   Err_ShowErrorAndExit ("Wrong holiday.");
  }

/*****************************************************************************/
/********** Write error message and exit when wrong building/room ************/
/*****************************************************************************/

void Err_WrongBuildingExit (void)
  {
   Err_ShowErrorAndExit ("Wrong building.");
  }

void Err_WrongRoomExit (void)
  {
   Err_ShowErrorAndExit ("Wrong room.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong record field ************/
/*****************************************************************************/

void Err_WrongRecordFieldExit (void)
  {
   Err_ShowErrorAndExit ("Wrong record field.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong assignment **************/
/*****************************************************************************/

void Err_WrongAssignmentExit (void)
  {
   Err_ShowErrorAndExit ("Wrong assignment.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong project ***************/
/*****************************************************************************/

void Err_WrongProjectExit (void)
  {
   Err_ShowErrorAndExit ("Wrong project.");
  }

/*****************************************************************************/
/********** Write error message and exit when wrong call for exam ************/
/*****************************************************************************/

void Err_WrongCallForExamExit (void)
  {
   Err_ShowErrorAndExit ("Wrong call for exam.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong tag *****************/
/*****************************************************************************/

void Err_WrongTagExit (void)
  {
   Err_ShowErrorAndExit ("Wrong tag.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong test *****************/
/*****************************************************************************/

void Err_WrongTestExit (void)
  {
   Err_ShowErrorAndExit ("Wrong test.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong exam *****************/
/*****************************************************************************/

void Err_WrongExamExit (void)
  {
   Err_ShowErrorAndExit ("Wrong exam.");
  }

/*****************************************************************************/
/********* Write error message and exit when wrong set of questions **********/
/*****************************************************************************/

void Err_WrongSetExit (void)
  {
   Err_ShowErrorAndExit ("Wrong set of questions.");
  }

/*****************************************************************************/
/************* Write error message and exit when wrong question **************/
/*****************************************************************************/

void Err_WrongQuestionExit (void)
  {
   Err_ShowErrorAndExit ("Wrong question.");
  }

void Err_WrongQuestionIndexExit (void)
  {
   Err_ShowErrorAndExit ("Wrong question index.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong answer ***************/
/*****************************************************************************/

void Err_WrongAnswerExit (void)
  {
   Err_ShowErrorAndExit ("Wrong answer.");
  }

void Err_WrongAnswerIndexExit (void)
  {
   Err_ShowErrorAndExit ("Wrong answer index.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong exam session ************/
/*****************************************************************************/

void Err_WrongExamSessionExit (void)
  {
   Err_ShowErrorAndExit ("Wrong exam session.");
  }

/*****************************************************************************/
/************ Write error message and exit when wrong game/match *************/
/*****************************************************************************/

void Err_WrongGameExit (void)
  {
   Err_ShowErrorAndExit ("Wrong game.");
  }

void Err_WrongMatchExit (void)
  {
   Err_ShowErrorAndExit ("Wrong match.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong survey ***************/
/*****************************************************************************/

void Err_WrongSurveyExit (void)
  {
   Err_ShowErrorAndExit ("Wrong survey.");
  }

/*****************************************************************************/
/*** Write error message and exit when wrong parameter "who" (which users) ***/
/*****************************************************************************/

void Err_WrongWhoExit (void)
  {
   Err_ShowErrorAndExit ("Wrong parameter who.");
  }

/*****************************************************************************/
/**************** Write error message and exit when wrong event **************/
/*****************************************************************************/

void Err_WrongEventExit (void)
  {
   Err_ShowErrorAndExit ("Wrong event.");
  }

/*****************************************************************************/
/*********** Write error message and exit when wrong announcement ************/
/*****************************************************************************/

void Err_WrongAnnouncementExit (void)
  {
   Err_ShowErrorAndExit ("Wrong announcement.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong notice ***************/
/*****************************************************************************/

void Err_WrongNoticeExit (void)
  {
   Err_ShowErrorAndExit ("Wrong notice.");
  }

/*****************************************************************************/
/************ Write error message and exit when wrong forum/post *************/
/*****************************************************************************/

void Err_WrongForumExit (void)
  {
   Err_ShowErrorAndExit ("Wrong forum.");
  }

void Err_WrongPostExit (void)
  {
   Err_ShowErrorAndExit ("Wrong post.");
  }

/*****************************************************************************/
/************** Write error message and exit when wrong message **************/
/*****************************************************************************/

void Err_WrongMessageExit (void)
  {
   Err_ShowErrorAndExit ("Wrong message.");
  }

/*****************************************************************************/
/************ Write error message and exit when wrong mail domain ************/
/*****************************************************************************/

void Err_WrongMailDomainExit (void)
  {
   Err_ShowErrorAndExit ("Wrong mail domain.");
  }

/*****************************************************************************/
/************* Show error message about wrong role and exit ******************/
/*****************************************************************************/

void Err_WrongRoleExit (void)
  {
   Err_ShowErrorAndExit ("Wrong role.");
  }

/*****************************************************************************/
/************ Write error message and exit when user does not exists *********/
/*****************************************************************************/

void Err_WrongUserExit (void)
  {
   extern const char *Txt_The_user_does_not_exist;

   Err_ShowErrorAndExit (Txt_The_user_does_not_exist);
  }

/*****************************************************************************/
/*** Write error message and exit when no permission to perform an action ****/
/*****************************************************************************/

void Err_NoPermissionExit (void)
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

   Err_ShowErrorAndExit (Txt_You_dont_have_permission_to_perform_this_action);
  }

/*****************************************************************************/
/******* Write error message, close files, remove lock file, and exit ********/
/*****************************************************************************/

void Err_ShowErrorAndExit (const char *Txt)
  {
   /***** Unlock tables if locked *****/
   if (Gbl.DB.LockedTables)
     {
      Gbl.DB.LockedTables = false;
      mysql_query (&Gbl.mysql,"UNLOCK TABLES");
     }

   if (!Gbl.WebService.IsWebService)
     {
      /****** If start of page is not written yet, do it now ******/
      if (!Gbl.Layout.HTMLStartWritten)
	 Lay_WriteStartOfPage ();

      /***** Write possible error message *****/
      if (Txt)
         Ale_ShowAlert (Ale_ERROR,Txt);

      /***** Finish the page, except </body> and </html> *****/
      Lay_WriteEndOfPage ();
     }

   /***** Free memory and close all the open files *****/
   Gbl_Cleanup ();

   /***** Page is generated (except </body> and </html>).
          Compute time to generate page *****/
   if (!Gbl.Action.IsAJAXAutoRefresh)
      Gbl.TimeGenerationInMicroseconds = (long) Sta_ComputeTimeToGeneratePage ();

   if (Gbl.WebService.IsWebService)		// Serving a plugin request
     {
      /***** Log access *****/
      Gbl.TimeSendInMicroseconds = 0L;
      Log_LogAccess (Txt);
     }
   else
     {
      /***** Send page.
             The HTML output is now in Gbl.F.Out file ==>
             ==> copy it to standard output *****/
      rewind (Gbl.F.Out);
      Fil_FastCopyOfOpenFiles (Gbl.F.Out,stdout);
      Fil_CloseAndRemoveFileForHTMLOutput ();

      if (!Gbl.Action.IsAJAXAutoRefresh)
	{
	 /***** Compute time to send page *****/
	 Sta_ComputeTimeToSendPage ();

	 /***** Log access *****/
	 Log_LogAccess (Txt);

	 /***** Update last data for next time *****/
	 if (Gbl.Usrs.Me.Logged)
	   {
	    Usr_UpdateMyLastData ();
	    Crs_UpdateCrsLast ();
	   }

	 /***** End the output *****/
	 if (!Gbl.Layout.HTMLEndWritten)
	   {
	    // Here Gbl.F.Out is stdout
	    if (Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB)
	       Lay_WriteAboutZone ();

	    HTM_Txt ("</body>\n"
		     "</html>\n");
	    Gbl.Layout.HTMLEndWritten = true;
	   }
	}
     }

   /***** Close database connection *****/
   DB_CloseDBConnection ();

   /***** Exit *****/
   if (Gbl.WebService.IsWebService)
      API_Exit (Txt);
   exit (0);
  }
