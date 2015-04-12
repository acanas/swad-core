// swad_import.c: import courses and students from another web application using web services

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_account.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_import.h"
#include "swad_parameter.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Imp_OK				0
#define Imp_ERROR_BAD_SESSION		1
#define Imp_ERROR_NOT_ENOUGH_MEMORY	2
#define Imp_ERROR_SOAP			3

#define Imp_MAX_BYTES_USR_ID 16
#define Imp_MAX_BYTES_NAME   32
#define Imp_MAX_BYTES_EMAIL 127

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct ImportedStudent
  {
   char ExternalUsrID[Imp_MAX_BYTES_USR_ID+1];
   char FirstName[Imp_MAX_BYTES_NAME+1];
   char Surname1[Imp_MAX_BYTES_NAME+1];
   char Surname2[Imp_MAX_BYTES_NAME+1];
   char Email[Imp_MAX_BYTES_EMAIL+1];
   Usr_Sex_t Sex;
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Imp_ListMyImpGrpsAndStds (bool ItsAFormToRegRemStds);
static void Imp_RemoveOldImpSessions (void);
static void Imp_RemoveOldImpStdsAndGrps (void);
static unsigned Imp_GetAndListImpGrpsAndStdsFromDB (bool ItsAFormToRegRemStds);

/*****************************************************************************/
/************** Get parameters with imported user and session ****************/
/*****************************************************************************/

void Imp_GetImpUsrAndSession (void)
  {
   Gbl.Imported.ExternalUsrId[0] = '\0';
   if (Cfg_NAME_PARAM_IMPORTED_USR_ID[0])
      Par_GetParToText (Cfg_NAME_PARAM_IMPORTED_USR_ID,Gbl.Imported.ExternalUsrId,Cfg_MAX_LENGTH_IMPORTED_USR_ID);

   Gbl.Imported.ExternalSesId[0] = '\0';
   if (Cfg_NAME_PARAM_IMPORTED_SESSION_ID[0])
      Par_GetParToText (Cfg_NAME_PARAM_IMPORTED_SESSION_ID,Gbl.Imported.ExternalSesId,Cfg_MAX_LENGTH_IMPORTED_SESSION_ID);
  }

/*****************************************************************************/
/********** Import the students of an official group from database ***********/
/*****************************************************************************/

void Imp_ImportStdsFromAnImpGrp (long ImpGrpCod,struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumStds;
   unsigned NumStd;
   struct ImportedStudent ImpStd;
   Usr_Sex_t Sex;
   struct UsrData UsrDat;
   struct ListUsrCods ListUsrCodsForThisID;
   unsigned NumUsrFoundForThisID;

   /***** Get official data of the imported students belonging to this group from database *****/
   sprintf (Query,"SELECT UsrID,Surname1,Surname2,FirstName,Sex,E_mail"
                  " FROM imported_students WHERE GrpCod='%ld'"
                  " ORDER BY Surname1,Surname2,FirstName,UsrID",
                  ImpGrpCod);
   NumStds = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get imported students");

   if (NumStds)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Import the students from this official group *****/
      for (NumStd = 0;
	   NumStd < NumStds;
	   NumStd++)	// For each student inside the official group...
        {
         /* Get official data of the student */
         row = mysql_fetch_row (mysql_res);

         /* Get user's ID (row[0]) */
         strncpy (ImpStd.ExternalUsrID,row[0],Imp_MAX_BYTES_USR_ID);
         ImpStd.ExternalUsrID[Imp_MAX_BYTES_USR_ID] = '\0';
         // Users' IDs are always stored internally in capitals and without leading zeros
	 Str_RemoveLeadingZeros (ImpStd.ExternalUsrID);
	 Str_ConvertToUpperText (ImpStd.ExternalUsrID);

         if (ID_CheckIfUsrIDIsValid (ImpStd.ExternalUsrID))	// If it's not a valid user's ID, skip it
           {
	    /* Get Surname1 (row[1]) */
	    strncpy (ImpStd.Surname1,row[1],Imp_MAX_BYTES_NAME);
	    ImpStd.Surname1[Imp_MAX_BYTES_NAME] = '\0';

	    /* Get Surname2 (row[2]) */
	    strncpy (ImpStd.Surname2,row[2],Imp_MAX_BYTES_NAME);
	    ImpStd.Surname2[Imp_MAX_BYTES_NAME] = '\0';

	    /* Get FirstName (row[3]) */
	    strncpy (ImpStd.FirstName,row[3],Imp_MAX_BYTES_NAME);
	    ImpStd.FirstName[Imp_MAX_BYTES_NAME] = '\0';

	    /* Get Sex (row[4]) */
	    ImpStd.Sex = Usr_SEX_UNKNOWN;
	    for (Sex = (Usr_Sex_t) 0;
		 Sex < Usr_NUM_SEXS;
		 Sex++)
	       if (!strcasecmp (row[4],Usr_StringsSexDB[Sex]))
		 {
		  ImpStd.Sex = Sex;
		  break;
		 }

	    /* Get e-mail (row[5]) */
	    strncpy (ImpStd.Email,row[5],Imp_MAX_BYTES_EMAIL);
	    ImpStd.Email[Imp_MAX_BYTES_EMAIL] = '\0';

            /* Set ID of the student */
            ID_ReallocateListIDs (&UsrDat,1);
	    strncpy (UsrDat.IDs.List[0].ID,ImpStd.ExternalUsrID,ID_MAX_LENGTH_USR_ID);
	    UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';

            if (ID_GetListUsrCodsFromUsrID (&UsrDat,NULL,&ListUsrCodsForThisID,false))
              {// User(s) found
               for (NumUsrFoundForThisID = 0;
        	    NumUsrFoundForThisID < ListUsrCodsForThisID.NumUsrs;
        	    NumUsrFoundForThisID++)
        	 {
        	  /* Get user's data */
        	  UsrDat.UsrCod = ListUsrCodsForThisID.Lst[NumUsrFoundForThisID];
		  Usr_GetUsrDataFromUsrCod (&UsrDat);	// Get data of the first user in list

		  if (UsrDat.RoleInCurrentCrsDB <= Rol_STUDENT)
		    {
		     /* Update user's data if only one user with this ID */
		     if (ListUsrCodsForThisID.NumUsrs == 1)	// Only one user with this ID
		       {
			/* Fill empty data */
			if (!UsrDat.Surname1[0])
			   strcpy (UsrDat.Surname1,ImpStd.Surname1);
			if (!UsrDat.Surname2[0])
			   strcpy (UsrDat.Surname2,ImpStd.Surname2);
			if (!UsrDat.FirstName[0])
			   strcpy (UsrDat.FirstName,ImpStd.FirstName);
			if (UsrDat.Sex == Usr_SEX_UNKNOWN)
			   UsrDat.Sex = ImpStd.Sex;

			/* Update user's data */
			Enr_UpdateUsrData (&UsrDat);

			/* Update e-mail */
			if (!UsrDat.Email[0])
			  {
			   strcpy (UsrDat.Email,ImpStd.Email);
			   if (!Mai_UpdateEmailInDB (&UsrDat,UsrDat.Email))	// Email was already registered and confirmed by another user
			      UsrDat.Email[0] = '\0';
			  }
		       }

		     if (UsrDat.RoleInCurrentCrsDB == Rol_STUDENT)	// He/she was already a student in current course
			Enr_AcceptUsrInCrs (UsrDat.UsrCod);
		     else						// He/she not belonged to the current course
			/* Register user as student in the current course */
			Enr_RegisterUsrInCurrentCrs (&UsrDat,Rol_STUDENT,
						     Cns_QUIET,Enr_SET_ACCEPTED_TO_TRUE);

		     /* Register user in the selected groups */
		     if (Gbl.CurrentCrs.Grps.NumGrps)			// If there are groups in current course
		        Grp_RegisterUsrIntoGroups (&UsrDat,LstGrps);	// Register student in the selected groups

		     /* Confirm user's ID */
		     ID_ConfirmUsrID (UsrDat.UsrCod,ImpStd.ExternalUsrID);

		     (*NumUsrsRegistered)++;
		    }
        	 }

               /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCodsForThisID);
              }
            else	// No users found with this ID
              {// The user does not exist in the platform. Register him/her in the current course as student
	       Usr_ResetUsrDataExceptUsrCodAndIDs (&UsrDat);
               strcpy (UsrDat.Surname1,ImpStd.Surname1);
               strcpy (UsrDat.Surname2,ImpStd.Surname2);
               strcpy (UsrDat.FirstName,ImpStd.FirstName);
               UsrDat.Sex = ImpStd.Sex;
               strcpy (UsrDat.Email,ImpStd.Email);

               /* Create user */
               UsrDat.IDs.List[0].Confirmed = true;	// If he/she is a new user ==> his/her ID will be stored as confirmed in database
               Acc_CreateNewUsr (&UsrDat);

               /* Update e-mail */
	       if (!Mai_UpdateEmailInDB (&UsrDat,UsrDat.Email))	// Email was already registered and confirmed by another user
		  UsrDat.Email[0] = '\0';

	       /* Register user as student in the current course */
	       Enr_RegisterUsrInCurrentCrs (&UsrDat,Rol_STUDENT,
					    Cns_QUIET,Enr_SET_ACCEPTED_TO_TRUE);

	       /* Register user in the selected groups */
	       if (Gbl.CurrentCrs.Grps.NumGrps)			// If there are groups in current course
		  Grp_RegisterUsrIntoGroups (&UsrDat,LstGrps);	// Register student in the selected groups

	       (*NumUsrsRegistered)++;
 	      }
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Insert new imported session in the database *****************/
/*****************************************************************************/

void Imp_InsertImpSessionInDB (void)
  {
   char Query[1024];

   /***** Remove old imported sessions *****/
   Imp_RemoveOldImpSessions ();

   /***** Insert session in the database *****/
   sprintf (Query,"INSERT INTO imported_sessions"
	          " (SessionId,UsrCod,ImportedUsrId,ImportedSessionId,ImportedRole)"
                  " VALUES ('%s','%ld','%s','%s','%u')",
            Gbl.Session.Id,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Gbl.Imported.ExternalUsrId,
            Gbl.Imported.ExternalSesId,
            (unsigned) Gbl.Imported.ExternalRole);
   DB_QueryINSERT (Query,"can not create imported session");
  }

/*****************************************************************************/
/******************** Get the data of a imported session *********************/
/*****************************************************************************/

bool Imp_GetImpSessionData (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool Result = false;

   /***** Get data of the imported session from database *****/
   sprintf (Query,"SELECT ImportedUsrId,ImportedSessionId,ImportedRole"
                  " FROM imported_sessions"
                  " WHERE SessionId='%s'",
            Gbl.Session.Id);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get imported session data");

   /***** Check if the session existed in the database *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get imported user identifier (row[0]) *****/
      strcpy (Gbl.Imported.ExternalUsrId,row[0]);

      /***** Get imported session identifier (row[1]) *****/
      strcpy (Gbl.Imported.ExternalSesId,row[1]);

      /***** Get imported user's (external) role (row[2]) *****/
      if (sscanf (row[2],"%u",&Gbl.Imported.ExternalRole) != 1)
         Gbl.Imported.ExternalRole = Rol_UNKNOWN;

      Result = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Result;
  }

/*****************************************************************************/
/************* Get lists of my imported groups and students ******************/
/*****************************************************************************/

void Imp_ListMyImpGrpsAndStdsNoForm (void)
  {
   Imp_ListMyImpGrpsAndStds (false);
  }

/*****************************************************************************/
/************* Get lists of my imported groups and students ******************/
/*****************************************************************************/

void Imp_ListMyImpGrpsAndStdsForm (void)
  {
   Imp_ListMyImpGrpsAndStds (true);
  }

/*****************************************************************************/
/************* Get lists of my imported groups and students ******************/
/*****************************************************************************/

static void Imp_ListMyImpGrpsAndStds (bool ItsAFormToRegRemStds)
  {
   extern const char *Txt_Could_not_get_the_official_list_of_your_groups_and_students_;
   extern const char *Txt_To_get_the_official_list_of_your_groups_and_students_;
   char PathRelParamsToCommandsPriv[PATH_MAX+1];
   char FileNameParams[PATH_MAX+1];
   FILE *FileParams;
   char Command[2048];
   int ReturnCode;

   /***** Try to get lists from database *****/
   if (!Imp_GetAndListImpGrpsAndStdsFromDB (ItsAFormToRegRemStds))
     {
      /***** Remove old imported students and groups *****/
      Imp_RemoveOldImpStdsAndGrps ();

      if (Gbl.Imported.ExternalUsrId[0] &&
          Gbl.Imported.ExternalSesId[0] &&
          Gbl.Imported.ExternalRole == Rol_TEACHER)
        {
         /***** Parameters to command used to import data are passed through a temporary file *****/
         /* If the private directory does not exist, create it */
         sprintf (PathRelParamsToCommandsPriv,"%s/%s",
                  Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PARAM);
         Fil_CreateDirIfNotExists (PathRelParamsToCommandsPriv);

         /* First of all, we remove the oldest temporary files.
            Such temporary files have been created by me or by other users.
            This is a bit sloppy, but they must be removed by someone.
            Here "oldest" means more than x time from their creation */
         Fil_RemoveOldTmpFiles (PathRelParamsToCommandsPriv,Cfg_TIME_TO_DELETE_PARAMS_TO_COMMANDS,false);

         /****** Create a new temporary file *****/
         sprintf (FileNameParams,"%s/%s",PathRelParamsToCommandsPriv,Gbl.UniqueNameEncrypted);
         if ((FileParams = fopen (FileNameParams,"wb")) == NULL)
            Lay_ShowErrorAndExit ("Can not open file to send parameters to command.");
         fprintf (FileParams,"2\n%s\n%s\n%s\n",
                  Gbl.Session.Id,Gbl.Imported.ExternalUsrId,Gbl.Imported.ExternalSesId);
         fclose (FileParams);

         /***** Get new imported lists and store them in database *****/
         sprintf (Command,"%s %s",Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND,FileNameParams);
         ReturnCode = system (Command);
         // unlink (FileNameParams);	// File with parameters is no longer necessary
         if (ReturnCode == -1)
            Lay_ShowErrorAndExit ("Error when running command to import lists of groups and students.");

         /***** Write message depending on return code *****/
         ReturnCode = WEXITSTATUS(ReturnCode);
         switch (ReturnCode)
           {
            case 0:	// Success
               /***** Try again to get lists from database *****/
               if (!Imp_GetAndListImpGrpsAndStdsFromDB (ItsAFormToRegRemStds))
                 {
                  sprintf (Gbl.Message,Txt_Could_not_get_the_official_list_of_your_groups_and_students_,
                           Cfg_PLATFORM_SHORT_NAME,
                           Cfg_EXTERNAL_LOGIN_URL,
                           Cfg_EXTERNAL_LOGIN_SERVICE_FULL_NAME);
                  Lay_ShowAlert (Lay_ERROR,Gbl.Message);
                 }
               break;
            default:	// Error
               sprintf (Gbl.Message,"Error %d while importing lists of groups and students.",ReturnCode);
               Lay_ShowAlert (Lay_ERROR,Gbl.Message);
               break;
           }
        }
      else	// I am not entered from external program as teacher
        {
         sprintf (Gbl.Message,Txt_To_get_the_official_list_of_your_groups_and_students_,
                  Cfg_PLATFORM_SHORT_NAME,
                  Cfg_EXTERNAL_LOGIN_URL,
                  Cfg_EXTERNAL_LOGIN_SERVICE_FULL_NAME);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
  }

/*****************************************************************************/
/*********************** Remove old imported sessions ************************/
/*****************************************************************************/

static void Imp_RemoveOldImpSessions (void)
  {
   char Query[1024];

   /***** Remove old imported sessions *****/
   sprintf (Query,"DELETE FROM imported_sessions"
	          " WHERE SessionId NOT IN (SELECT SessionId FROM sessions)");
   DB_QueryDELETE (Query,"can not remove old imported sessions");
  }

/*****************************************************************************/
/***************** Remove old imported students and groups *******************/
/*****************************************************************************/

static void Imp_RemoveOldImpStdsAndGrps (void)
  {
   char Query[1024];

   /***** Remove old imported students *****/
   sprintf (Query,"DELETE FROM imported_students"
	          " USING imported_groups,imported_students"
                  " WHERE imported_groups.SessionId NOT IN"
                  " (SELECT SessionId FROM sessions)"
                  " AND imported_groups.GrpCod=imported_students.GrpCod");
   DB_QueryDELETE (Query,"can not remove old imported students");

   /***** Remove old imported groups *****/
   sprintf (Query,"DELETE FROM imported_groups"
	          " WHERE SessionId NOT IN"
	          " (SELECT SessionId FROM sessions)");
   DB_QueryDELETE (Query,"can not remove old imported groups");
  }

/*****************************************************************************/
/******** Get from database and list imported groups and students ************/
/*****************************************************************************/

static unsigned Imp_GetAndListImpGrpsAndStdsFromDB (bool ItsAFormToRegRemStds)
  {
   extern const char *Txt_Official_students;
   extern const char *Txt_Group;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[1024];
   MYSQL_RES *mysql_res_grp;
   MYSQL_RES *mysql_res_std;
   MYSQL_ROW row;
   char ExternalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD+1];
   unsigned NumGrps;
   unsigned NumGrp;
   long GrpCod;
   unsigned NumStds;
   unsigned NumStd;

   /***** Get data of imported groups from database *****/
   sprintf (Query,"SELECT GrpCod,ExternalCrsCod,DegName,CrsName,GrpName,GrpType"
	          " FROM imported_groups"
                  " WHERE SessionId='%s'"
                  " ORDER BY DegName,CrsName,GrpName,GrpType",
            Gbl.Session.Id);
   NumGrps = (unsigned) DB_QuerySELECT (Query,&mysql_res_grp,"can not get imported groups");

   if (NumGrps)
     {
      /***** Start table with groups *****/
      Lay_StartRoundFrameTable (NULL,2,NULL);
      fprintf (Gbl.F.Out,"<tr>"
	                 "<th colspan=\"%u\" class=\"TIT_TBL\""
	                 " style=\"text-align:center; vertical-align:top;\">",
	       ItsAFormToRegRemStds ? 3 :
		                      2);
      Lay_WriteTitle (Txt_Official_students);
      fprintf (Gbl.F.Out,"</th>"
			 "</tr>");

      /***** Get groups and write them *****/
      for (NumGrp = 0;
	   NumGrp < NumGrps;
	   NumGrp++)	// For each course
        {
         row = mysql_fetch_row (mysql_res_grp);

         GrpCod = Str_ConvertStrCodToLongCod (row[0]);
         strncpy (ExternalCrsCod,row[1],Crs_LENGTH_INSTITUTIONAL_CRS_COD);
         ExternalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD] = '\0';

         /***** Get data of the imported students belonging to this group from database *****/
         sprintf (Query,"SELECT UsrID,Surname1,Surname2,FirstName"
                        " FROM imported_students"
                        " WHERE GrpCod='%ld' ORDER BY Surname1,Surname2,FirstName,UsrID",
                  GrpCod);
         NumStds = (unsigned) DB_QuerySELECT (Query,&mysql_res_std,"can not get imported students");

         fprintf (Gbl.F.Out,"<tr>");
         if (ItsAFormToRegRemStds)
            /* Put checkbox to select the group */
            fprintf (Gbl.F.Out,"<td rowspan=\"2\" style=\"text-align:left;"
        	               " vertical-align:top;\">"
                               "<input type=\"checkbox\" name=\"ImpGrpCod\" value=\"%ld_%s\" />"
                               "</td>",
                     GrpCod,ExternalCrsCod);

         /* Write degree, course and group */
         fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"TIT_TBL\""
                            " style=\"text-align:left; vertical-align:top;\">"
                            "%s<br />%s %s<br />%s %s (%s)"
                            "</td>" \
                            "</tr>",
                  row[2],
                  ExternalCrsCod,row[3],
                  Txt_Group,row[4],row[5]);

         /* Write students */
         fprintf (Gbl.F.Out,"<tr>" \
                            "<td class=\"DAT\""
                            " style=\"text-align:left; vertical-align:top;\">"
                            "%u %s%s"
                            "</td>" \
                            "<td class=\"DAT\""
                            " style=\"text-align:left; vertical-align:top;\">",
                  NumStds,Txt_ROLES_PLURAL_abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
                  NumStds ? ":" :
                	    "");
         if (NumStds)
           {
            fprintf (Gbl.F.Out,"<textarea cols=\"60\" rows=\"%u\" readonly>",
                     NumStds > 50 ? NumStds / 10 :
                	            5);
            for (NumStd = 0;
        	 NumStd < NumStds;
        	 NumStd++)	// For each student inside the group
              {
               row = mysql_fetch_row (mysql_res_std);
               fprintf (Gbl.F.Out,"%s %s",row[0],row[1]);	// User's ID and Surname 1
               if (row[2][0])
                  fprintf (Gbl.F.Out," %s",row[2]);	// Surname 2
               fprintf (Gbl.F.Out,", %s\n",row[3]);	// First name
              }
            fprintf (Gbl.F.Out,"</textarea>");
           }
         fprintf (Gbl.F.Out,"</td>" \
                            "</tr>");

         /***** Free structure that stores the query result *****/
         DB_FreeMySQLResult (&mysql_res_std);
        }

      /***** End of table with courses *****/
      Lay_EndRoundFrameTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res_grp);

   return NumGrps;
  }
