// swad_survey_database.c: surveys, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_survey.h"
#include "swad_survey_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

const char *Svy_DB_StrAnswerTypes[Svy_NUM_ANS_TYPES] =
  {
   [Svy_ANS_UNIQUE_CHOICE  ] = "unique_choice",
   [Svy_ANS_MULTIPLE_CHOICE] = "multiple_choice",
  };

/*****************************************************************************/
/*************************** Create a new survey *****************************/
/*****************************************************************************/

long Svy_DB_CreateSurvey (const struct Svy_Survey *Svy,const char *Txt)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new survey",
				"INSERT INTO svy_surveys"
				" (Scope,Cod,Hidden,Roles,UsrCod,"
				  "StartTime,EndTime,"
				  "Title,Txt)"
				" VALUES"
				" ('%s',%ld,'N',%u,%ld,"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s')",
				Hie_GetDBStrFromLevel (Svy->HieLvl),
				Svy->HieCod,
				Svy->Roles,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Svy->TimeUTC[Dat_STR_TIME],
				Svy->TimeUTC[Dat_END_TIME],
				Svy->Title,
				Txt);
  }

/*****************************************************************************/
/************************* Update an existing survey *************************/
/*****************************************************************************/

void Svy_DB_UpdateSurvey (const struct Svy_Survey *Svy,const char *Txt)
  {
   DB_QueryUPDATE ("can not update survey",
		   "UPDATE svy_surveys"
	             " SET Scope='%s',"
	                  "Cod=%ld,"
	                  "Roles=%u,"
	                  "StartTime=FROM_UNIXTIME(%ld),"
	                  "EndTime=FROM_UNIXTIME(%ld),"
	                  "Title='%s',"
	                  "Txt='%s'"
                   " WHERE SvyCod=%ld",
		   Hie_GetDBStrFromLevel (Svy->HieLvl),
		   Svy->HieCod,
		   Svy->Roles,
		   Svy->TimeUTC[Dat_STR_TIME],
		   Svy->TimeUTC[Dat_END_TIME],
		   Svy->Title,
		   Txt,
		   Svy->SvyCod);
  }

/*****************************************************************************/
/*************************** Hide/unhide a survey ****************************/
/*****************************************************************************/

void Svy_DB_HideOrUnhideSurvey (long SvyCod,
				HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_Hidden_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not hide/unhide survey",
		   "UPDATE svy_surveys"
		     " SET Hidden='%c'"
		   " WHERE SvyCod=%ld",
		   HidVis_Hidden_YN[HiddenOrVisible],
		   SvyCod);
  }

/*****************************************************************************/
/*********** Update number of users notified in table of surveys *************/
/*****************************************************************************/

void Svy_DB_UpdateNumUsrsNotifiedByEMailAboutSurvey (long SvyCod,
                                                     unsigned NumUsrsToBeNotifiedByEMail)
  {
   DB_QueryUPDATE ("can not update the number of notifications of a survey",
		   "UPDATE svy_surveys"
		     " SET NumNotif=NumNotif+%u"
                   " WHERE SvyCod=%ld",
		   NumUsrsToBeNotifiedByEMail,
		   SvyCod);
  }

/*****************************************************************************/
/************************* Get list of all surveys ***************************/
/*****************************************************************************/

unsigned Svy_DB_GetListSurveys (MYSQL_RES **mysql_res,
                                unsigned AllowedLvls,
                                unsigned HiddenAllowed,
                                Dat_StartEndTime_t SelectedOrder)
  {
   char *SubQuery[Hie_NUM_LEVELS];
   static const char *OrderBySubQuery[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = "StartTime DESC,"
	               "EndTime DESC,"
	               "Title DESC",
      [Dat_END_TIME] = "EndTime DESC,"
	               "StartTime DESC,"
	               "Title DESC",
     };
   unsigned NumSvys;
   Hie_Level_t HieLvl;
   bool SubQueryFilled = false;

   /***** Fill subqueries for system, country, institution, center and degree *****/
   for (HieLvl  = Hie_SYS;
	HieLvl <= Hie_DEG;
	HieLvl++)
      if (AllowedLvls & 1 << HieLvl)
	{
	 if (asprintf (&SubQuery[HieLvl],"%s(Scope='%s' AND Cod=%ld%s)",
		       SubQueryFilled ? " OR " :
					"",
		       Hie_GetDBStrFromLevel (HieLvl),Gbl.Hierarchy.Node[HieLvl].HieCod,
		       (HiddenAllowed & 1 << HieLvl) ? "" :
						      " AND Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 SubQueryFilled = true;
	}
      else
        {
	 if (asprintf (&SubQuery[HieLvl],"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
        }

   /***** Fill subquery for course *****/
   if (AllowedLvls & 1 << Hie_CRS)
     {
      if (Gbl.Crs.Grps.MyAllGrps == Grp_MY_GROUPS)
        {
	 if (asprintf (&SubQuery[Hie_CRS],"%s("
					     "Scope='%s'"
					     " AND Cod=%ld%s"
					     " AND "
					     "(SvyCod NOT IN"
					     " (SELECT SvyCod"
						" FROM svy_groups)"
					     " OR"
					     " SvyCod IN"
					     " (SELECT svy_groups.SvyCod"
						" FROM grp_users,"
						      "svy_groups"
					       " WHERE grp_users.UsrCod=%ld"
						 " AND grp_users.GrpCod=svy_groups.GrpCod))"
					     ")",
		       SubQueryFilled ? " OR " :
					"",
		       Hie_GetDBStrFromLevel (Hie_CRS),Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		       (HiddenAllowed & 1 << Hie_CRS) ? "" :
							   " AND Hidden='N'",
		       Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	    Err_NotEnoughMemoryExit ();
        }
      else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
        {
	 if (asprintf (&SubQuery[Hie_CRS],"%s(Scope='%s' AND Cod=%ld%s)",
		       SubQueryFilled ? " OR " :
					"",
		       Hie_GetDBStrFromLevel (Hie_CRS),Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		       (HiddenAllowed & 1 << Hie_CRS) ? "" :
							   " AND Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
        }
      SubQueryFilled = true;
     }
   else
     {
      if (asprintf (&SubQuery[Hie_CRS],"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /***** Make query *****/
   if (SubQueryFilled)
      NumSvys = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get surveys",
		      "SELECT SvyCod"
		       " FROM svy_surveys"
		      " WHERE %s%s%s%s%s%s"
		   " ORDER BY %s",
		      SubQuery[Hie_SYS],
		      SubQuery[Hie_CTY],
		      SubQuery[Hie_INS],
		      SubQuery[Hie_CTR],
		      SubQuery[Hie_DEG],
		      SubQuery[Hie_CRS],
		      OrderBySubQuery[SelectedOrder]);
   else
     {
      Err_ShowErrorAndExit ("Can not get list of surveys.");
      NumSvys = 0;	// Not reached. Initialized to avoid warning
     }

   /***** Free allocated memory for subqueries *****/
   for (HieLvl  = Hie_SYS;
	HieLvl <= Hie_CRS;
	HieLvl++)
      free (SubQuery[HieLvl]);

   return NumSvys;
  }

/*****************************************************************************/
/********************* Get survey data using its code ************************/
/*****************************************************************************/

unsigned Svy_DB_GetSurveyDataByCod (MYSQL_RES **mysql_res,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get survey data",
		   "SELECT SvyCod,"					// row[0]
			  "Scope,"					// row[1]
			  "Cod,"					// row[2]
			  "Hidden,"					// row[3]
			  "Roles,"					// row[4]
			  "UsrCod,"					// row[5]
			  "UNIX_TIMESTAMP(StartTime),"			// row[6]
			  "UNIX_TIMESTAMP(EndTime),"			// row[7]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[8]
			  "Title"
		    " FROM svy_surveys"
		   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/******************** Get survey title using its code ************************/
/*****************************************************************************/

void Svy_DB_GetSurveyTitle (long SvyCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get survey title",
		         "SELECT Title"	// row[0]
			  " FROM svy_surveys"
		         " WHERE SvyCod=%ld",	// Extra check
			 SvyCod);
  }

/*****************************************************************************/
/*************** Get survey title and text using its code ********************/
/*****************************************************************************/

unsigned Svy_DB_GetSurveyTitleAndText (MYSQL_RES **mysql_res,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of a survey",
		   "SELECT Title,"	// row[0]
			  "Txt"		// row[1]
		    " FROM svy_surveys"
		   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/********************** Get survey text from database ************************/
/*****************************************************************************/

void Svy_DB_GetSurveyTxt (long SvyCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Txt,Cns_MAX_BYTES_TEXT,"can not get survey text",
		         "SELECT Txt"
			  " FROM svy_surveys"
		         " WHERE SvyCod=%ld",
		         SvyCod);
  }

/*****************************************************************************/
/******************* Check if the title of a survey exists *******************/
/*****************************************************************************/

bool Svy_DB_CheckIfSimilarSurveyExists (const struct Svy_Survey *Svy)
  {
   return
   DB_QueryEXISTS ("can not get similar surveys",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM svy_surveys"
		    " WHERE Scope='%s'"
		      " AND Cod=%ld"
		      " AND Title='%s'"
		      " AND SvyCod<>%ld)",
		   Hie_GetDBStrFromLevel (Svy->HieLvl),
		   Svy->HieCod,
		   Svy->Title,
		   Svy->SvyCod);
  }

/*****************************************************************************/
/******************** Get number of courses with surveys *********************/
/*****************************************************************************/
// Returns the number of courses with surveys for courses
// in this location (all the platform, current degree or current course)

unsigned Svy_DB_GetNumCrssWithCrsSurveys (Hie_Level_t HieLvl)
  {
   /***** Get number of courses with surveys from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with surveys",
			 "SELECT COUNT(DISTINCT Cod)"
                          " FROM svy_surveys"
                         " WHERE Scope='%s'",
			 Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with surveys",
			"SELECT COUNT(DISTINCT svy_surveys.Cod)"
                         " FROM ins_instits,"
                               "ctr_centers,"
                               "deg_degrees,"
                               "crs_courses,"
                               "svy_surveys"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
                          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                          " AND deg_degrees.DegCod=crs_courses.DegCod"
                          " AND crs_courses.CrsCod=svy_surveys.Cod"
                          " AND svy_surveys.Scope='%s'",
			Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with surveys",
		        "SELECT COUNT(DISTINCT svy_surveys.Cod)"
		         " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "svy_surveys"
		        " WHERE ctr_centers.InsCod=%ld"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=svy_surveys.Cod"
		          " AND svy_surveys.Scope='%s'",
		        Gbl.Hierarchy.Node[Hie_INS].HieCod,
		        Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with surveys",
			"SELECT COUNT(DISTINCT svy_surveys.Cod)"
                         " FROM deg_degrees,"
                               "crs_courses,"
                               "svy_surveys"
                        " WHERE deg_degrees.CtrCod=%ld"
                          " AND deg_degrees.DegCod=crs_courses.DegCod"
                          " AND crs_courses.CrsCod=svy_surveys.Cod"
                          " AND svy_surveys.Scope='%s'",
			Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with surveys",
			"SELECT COUNT(DISTINCT svy_surveys.Cod)"
                        " FROM crs_courses,"
                              "svy_surveys"
                        " WHERE crs_courses.DegCod=%ld"
                        " AND crs_courses.CrsCod=svy_surveys.Cod"
                        " AND svy_surveys.Scope='%s'",
		 	Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with surveys",
			"SELECT COUNT(DISTINCT Cod)"
			 " FROM svy_surveys"
			" WHERE Scope='%s'"
			  " AND Cod=%ld",
			Hie_GetDBStrFromLevel (Hie_CRS),
			Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******************** Get number of surveys for courses **********************/
/*****************************************************************************/
// Returns the number of surveys for courses
// in this location (all the platform, current degree or current course)

unsigned Svy_DB_GetNumCrsSurveys (MYSQL_RES **mysql_res,Hie_Level_t HieLvl)
  {
   /***** Get number of surveys from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of surveys",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(NumNotif)"			// row[1]
                          " FROM svy_surveys"
                         " WHERE Scope='%s'",
			 Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CTY:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of surveys",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(svy_surveys.NumNotif)"	// row[1]
                          " FROM ins_instits,"
                                "ctr_centers,"
                                "deg_degrees,"
                                "crs_courses,"
                                "svy_surveys"
                         " WHERE ins_instits.CtyCod=%ld"
                           " AND ins_instits.InsCod=ctr_centers.InsCod"
                           " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                           " AND deg_degrees.DegCod=crs_courses.DegCod"
                           " AND crs_courses.CrsCod=svy_surveys.Cod"
                           " AND svy_surveys.Scope='%s'",
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			 Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_INS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of surveys",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(svy_surveys.NumNotif)"	// row[1]
                          " FROM ctr_centers,"
                                "deg_degrees,"
                                "crs_courses,"
                                "svy_surveys"
                         " WHERE ctr_centers.InsCod=%ld"
                           " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
                           " AND deg_degrees.DegCod=crs_courses.DegCod"
                           " AND crs_courses.CrsCod=svy_surveys.Cod"
                           " AND svy_surveys.Scope='%s'",
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CTR:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of surveys",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(svy_surveys.NumNotif)"	// row[1]
                          " FROM deg_degrees,"
                                "crs_courses,"
                                "svy_surveys"
                         " WHERE deg_degrees.CtrCod=%ld"
                           " AND deg_degrees.DegCod=crs_courses.DegCod"
                           " AND crs_courses.CrsCod=svy_surveys.Cod"
                           " AND svy_surveys.Scope='%s'",
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			 Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_DEG:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of surveys",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(svy_surveys.NumNotif)"	// row[1]
                          " FROM crs_courses,"
                                "svy_surveys"
                         " WHERE crs_courses.DegCod=%ld"
                           " AND crs_courses.CrsCod=svy_surveys.Cod"
                           " AND svy_surveys.Scope='%s'",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			 Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CRS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of surveys",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(NumNotif)"			// row[1]
                          " FROM svy_surveys"
                         " WHERE svy_surveys.Scope='%s'"
                           " AND CrsCod=%ld",
			 Hie_GetDBStrFromLevel (Hie_CRS),
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************ Get average number of questions per course survey **************/
/*****************************************************************************/

double Svy_DB_GetNumQstsPerCrsSurvey (Hie_Level_t HieLvl)
  {
   /***** Get number of questions per survey from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
         return DB_QuerySELECTDouble ("can not get number of questions per survey",
				      "SELECT AVG(NumQsts)"
				       " FROM (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
					       " FROM svy_surveys,"
						     "svy_questions"
					      " WHERE svy_surveys.Scope='%s'"
					        " AND svy_surveys.SvyCod=svy_questions.SvyCod"
					   " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
				      Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CTY:
         return DB_QuerySELECTDouble ("can not get number of questions per survey",
				      "SELECT AVG(NumQsts)"
				       " FROM (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
					       " FROM ins_instits,"
						     "ctr_centers,"
						     "deg_degrees,"
						     "crs_courses,"
						     "svy_surveys,"
						     "svy_questions"
					      " WHERE ins_instits.CtyCod=%ld"
					        " AND ins_instits.InsCod=ctr_centers.InsCod"
					        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
					        " AND deg_degrees.DegCod=crs_courses.DegCod"
					        " AND crs_courses.CrsCod=svy_surveys.Cod"
					        " AND svy_surveys.Scope='%s'"
					        " AND svy_surveys.SvyCod=svy_questions.SvyCod"
					   " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
				      Gbl.Hierarchy.Node[Hie_CTY].HieCod,
				      Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_INS:
         return DB_QuerySELECTDouble ("can not get number of questions per survey",
				      "SELECT AVG(NumQsts)"
				       " FROM (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
					       " FROM ctr_centers,"
						     "deg_degrees,"
						     "crs_courses,"
						     "svy_surveys,"
						     "svy_questions"
					      " WHERE ctr_centers.InsCod=%ld"
					        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
					        " AND deg_degrees.DegCod=crs_courses.DegCod"
					        " AND crs_courses.CrsCod=svy_surveys.Cod"
					        " AND svy_surveys.Scope='%s'"
					        " AND svy_surveys.SvyCod=svy_questions.SvyCod"
					   " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
				      Gbl.Hierarchy.Node[Hie_INS].HieCod,
				      Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CTR:
         return DB_QuerySELECTDouble ("can not get number of questions per survey",
				      "SELECT AVG(NumQsts)"
				       " FROM (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
					       " FROM deg_degrees,"
						     "crs_courses,"
						     "svy_surveys,"
						     "svy_questions"
					      " WHERE deg_degrees.CtrCod=%ld"
					        " AND deg_degrees.DegCod=crs_courses.DegCod"
					        " AND crs_courses.CrsCod=svy_surveys.Cod"
					        " AND svy_surveys.Scope='%s'"
					        " AND svy_surveys.SvyCod=svy_questions.SvyCod"
					   " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
				      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				      Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_DEG:
         return DB_QuerySELECTDouble ("can not get number of questions per survey",
				      "SELECT AVG(NumQsts)"
				       " FROM (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
					       " FROM crs_courses,"
						     "svy_surveys,"
						     "svy_questions"
					      " WHERE crs_courses.DegCod=%ld"
					        " AND crs_courses.CrsCod=svy_surveys.Cod"
					        " AND svy_surveys.Scope='%s'"
					        " AND svy_surveys.SvyCod=svy_questions.SvyCod"
					   " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
				      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				      Hie_GetDBStrFromLevel (Hie_CRS));
      case Hie_CRS:
         return DB_QuerySELECTDouble ("can not get number of questions per survey",
				      "SELECT AVG(NumQsts)"
				       " FROM (SELECT COUNT(svy_questions.QstCod) AS NumQsts"
					       " FROM svy_surveys,"
						     "svy_questions"
					      " WHERE svy_surveys.Scope='%s'"
					        " AND svy_surveys.Cod=%ld"
					        " AND svy_surveys.SvyCod=svy_questions.SvyCod"
					   " GROUP BY svy_questions.SvyCod) AS NumQstsTable",
				      Hie_GetDBStrFromLevel (Hie_CRS),Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0.0;	// Not reached
     }
  }

/*****************************************************************************/
/************ Get all user codes belonging to a survey, except me ************/
/*****************************************************************************/

unsigned Svy_DB_GetUsrsFromSurveyExceptMe (MYSQL_RES **mysql_res,long SvyCod)
  {
   // 1. If the survey is available for the whole course ==> get users enroled in the course whose role is available in survey, except me
   // 2. If the survey is available only for some groups ==> get users who belong to any of the groups and whose role is available in survey, except me
   // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users to be notified",
		   "(SELECT crs_users.UsrCod"
		     " FROM svy_surveys,"
			   "crs_users"
		    " WHERE svy_surveys.SvyCod=%ld"
		      " AND svy_surveys.SvyCod NOT IN"
			  " (SELECT SvyCod"
			     " FROM svy_groups"
			    " WHERE SvyCod=%ld)"
		      " AND svy_surveys.Scope='%s'"
		      " AND svy_surveys.Cod=crs_users.CrsCod"
		      " AND crs_users.UsrCod<>%ld"
		      " AND (svy_surveys.Roles&(1<<crs_users.Role))<>0)"
		    " UNION "
		   "(SELECT DISTINCT "
		           "grp_users.UsrCod"
		     " FROM svy_groups,"
			   "grp_users,"
			   "svy_surveys,"
			   "crs_users"
		    " WHERE svy_groups.SvyCod=%ld"
		      " AND svy_groups.GrpCod=grp_users.GrpCod"
		      " AND grp_users.UsrCod=crs_users.UsrCod"
		      " AND grp_users.UsrCod<>%ld"
		      " AND svy_groups.SvyCod=svy_surveys.SvyCod"
		      " AND svy_surveys.Scope='%s'"
		      " AND svy_surveys.Cod=crs_users.CrsCod"
		      " AND (svy_surveys.Roles&(1<<crs_users.Role))<>0)",
		   SvyCod,
		   SvyCod,
		   Hie_GetDBStrFromLevel (Hie_CRS),
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SvyCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Hie_GetDBStrFromLevel (Hie_CRS));
  }

/*****************************************************************************/
/****************************** Remove a survey ******************************/
/*****************************************************************************/

void Svy_DB_RemoveSvy (long SvyCod)
  {
   DB_QueryDELETE ("can not remove survey",
		   "DELETE FROM svy_surveys"
		   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/************* Remove all surveys of a place on the hierarchy   **************/
/************* (country, institution, center, degree or course) **************/
/*****************************************************************************/

void Svy_DB_RemoveSvysIn (Hie_Level_t HieLvl,long HieCod)
  {
   DB_QueryDELETE ("can not remove all surveys in a place on the hierarchy",
		   "DELETE FROM svy_surveys"
	           " WHERE Scope='%s'"
	             " AND Cod=%ld",
		   Hie_GetDBStrFromLevel (HieLvl),
		   HieCod);
  }

/*****************************************************************************/
/************************ Create groups of a survey **************************/
/*****************************************************************************/

void Svy_DB_CreateGrp (long SvyCod,long GrpCod)
  {
   DB_QueryINSERT ("can not associate a group to a survey",
		   "INSERT INTO svy_groups"
		   " (SvyCod,GrpCod)"
		   " VALUES"
		   " (%ld,%ld)",
		   SvyCod,
		   GrpCod);
  }

/*****************************************************************************/
/****************** Get group names associated to a survey *******************/
/*****************************************************************************/

unsigned Svy_DB_GetGrpNamesAssociatedToSvy (MYSQL_RES **mysql_res,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of a survey",
		   "SELECT grp_types.GrpTypName,"	// row[0]
			  "grp_groups.GrpName"		// row[1]
		    " FROM svy_groups,"
			  "grp_groups,"
			  "grp_types"
		   " WHERE svy_groups.SvyCod=%ld"
		     " AND svy_groups.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		" ORDER BY grp_types.GrpTypName,"
			  "grp_groups.GrpName",
		   SvyCod);
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a survey *************/
/*****************************************************************************/

Usr_Can_t Svy_DB_CheckIfICanDoThisSurveyBasedOnGrps (long SvyCod)
  {
   return
   DB_QueryEXISTS ("can not check if I can do a survey",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM svy_surveys"
		    " WHERE SvyCod=%ld"
		      " AND (SvyCod NOT IN"
			   " (SELECT SvyCod"
			      " FROM svy_groups)"
			   " OR"
			   " SvyCod IN"
			   " (SELECT svy_groups.SvyCod"
			      " FROM grp_users,"
				    "svy_groups"
			     " WHERE grp_users.UsrCod=%ld"
			       " AND grp_users.GrpCod=svy_groups.GrpCod)))",
		   SvyCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_CAN :
						Usr_CAN_NOT;
  }

/*****************************************************************************/
/***************** Remove groups of one type from all surveys ****************/
/*****************************************************************************/

void Svy_DB_RemoveGroupsOfType (long GrpTypCod)
  {
   DB_QueryDELETE ("can not remove groups of a type"
	           " from the associations between surveys and groups",
		   "DELETE FROM svy_groups"
		   " USING grp_groups,"
		          "svy_groups"
                   " WHERE grp_groups.GrpTypCod=%ld"
                     " AND grp_groups.GrpCod=svy_groups.GrpCod",
		   GrpTypCod);
  }

/*****************************************************************************/
/********************* Remove one group from all surveys *********************/
/*****************************************************************************/

void Svy_DB_RemoveGroup (long GrpCod)
  {
   DB_QueryDELETE ("can not remove group from the associations"
		   " between surveys and groups",
		   "DELETE FROM svy_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/************************* Remove groups of a survey *************************/
/*****************************************************************************/

void Svy_DB_RemoveGrpsAssociatedToSurvey (long SvyCod)
  {
   DB_QueryDELETE ("can not remove the groups associated to a survey",
		   "DELETE FROM svy_groups"
		   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/********** Remove groups of all surveys of a place on the hierarchy *********/
/********** (country, institution, center, degree or course)         *********/
/*****************************************************************************/

void Svy_DB_RemoveGrpsSvysIn (Hie_Level_t HieLvl,long HieCod)
  {
   DB_QueryDELETE ("can not remove all groups"
	           " associated to surveys of a course",
		   "DELETE FROM svy_groups"
	           " USING svy_surveys,"
	                  "svy_groups"
                   " WHERE svy_surveys.Scope='%s'"
                     " AND svy_surveys.Cod=%ld"
                     " AND svy_surveys.SvyCod=svy_groups.SvyCod",
		   Hie_GetDBStrFromLevel (HieLvl),
		   HieCod);
  }

/*****************************************************************************/
/*************************** Create a new question ***************************/
/*****************************************************************************/

long Svy_DB_CreateQuestion (long SvyCod,
                            const struct Svy_Question *SvyQst,
                            const char Stem[Cns_MAX_BYTES_TEXT + 1])
  {
   extern const char DenAll_Allow_YN[DenAll_NUM_DENY_ALLOW];

   return
   DB_QueryINSERTandReturnCode ("can not create question",
				"INSERT INTO svy_questions"
				" (SvyCod,QstInd,AnsType,CommentsAllowed,Stem)"
				" VALUES"
				" (%ld,%u,'%s','%c','%s')",
				SvyCod,
				SvyQst->QstInd,
				Svy_DB_StrAnswerTypes[SvyQst->AnswerType],
				DenAll_Allow_YN[SvyQst->AllowCommentsByStds],
				Stem);
  }

/*****************************************************************************/
/************************ Create an existing question ************************/
/*****************************************************************************/

void Svy_DB_UpdateQuestion (long SvyCod,
                            const struct Svy_Question *SvyQst,
                            const char Stem[Cns_MAX_BYTES_TEXT + 1])
  {
   extern const char DenAll_Allow_YN[DenAll_NUM_DENY_ALLOW];

   DB_QueryUPDATE ("can not update question",
		   "UPDATE svy_questions"
		     " SET Stem='%s',"
			  "AnsType='%s',"
                          "CommentsAllowed='%c'"
		   " WHERE QstCod=%ld"
		     " AND SvyCod=%ld",	// Extra check
		   Stem,
		   Svy_DB_StrAnswerTypes[SvyQst->AnswerType],
		   DenAll_Allow_YN[SvyQst->AllowCommentsByStds],
		   SvyQst->QstCod,
		   SvyCod);
  }

/*****************************************************************************/
/*********** Change indexes of questions greater than a given one ************/
/*****************************************************************************/

void Svy_DB_ChangeIndexesQsts (long SvyCod,unsigned QstInd)
  {
   DB_QueryUPDATE ("can not update indexes of questions",
		   "UPDATE svy_questions"
		     " SET QstInd=QstInd-1"
                   " WHERE SvyCod=%ld"
                     " AND QstInd>%u",
		   SvyCod,
		   QstInd);
  }

/*****************************************************************************/
/******************* Get number of questions of a survey *********************/
/*****************************************************************************/

unsigned Svy_DB_GetNumQstsSvy (long SvyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of questions of a survey",
		  "SELECT COUNT(*)"
		   " FROM svy_questions"
		  " WHERE SvyCod=%ld",
		  SvyCod);
  }

/*****************************************************************************/
/***************** Get questions of a survey from database *******************/
/*****************************************************************************/

unsigned Svy_DB_GetSurveyQstsCodes (MYSQL_RES **mysql_res,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions of a survey",
		   "SELECT QstCod"
		    " FROM svy_questions"
		   " WHERE SvyCod=%ld"
		" ORDER BY QstCod",
		   SvyCod);
  }

/*****************************************************************************/
/***************** Get questions of a survey from database *******************/
/*****************************************************************************/

unsigned Svy_DB_GetSurveyQsts (MYSQL_RES **mysql_res,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of questions of a survey",
		   "SELECT QstCod,"		// row[0]
		          "QstInd,"		// row[1]
		          "AnsType,"		// row[2]
                          "CommentsAllowed,"	// row[3]
		          "Stem"		// row[4]
		    " FROM svy_questions"
		   " WHERE SvyCod=%ld"
	        " ORDER BY QstInd",
		   SvyCod);
  }

/*****************************************************************************/
/************ Get question data from question code in a survey ***************/
/*****************************************************************************/

unsigned Svy_DB_GetQstDataByCod (MYSQL_RES **mysql_res,long QstCod,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get a question",
		   "SELECT QstCod,"		// row[0]
		          "QstInd,"		// row[1]
		          "AnsType,"		// row[2]
                          "CommentsAllowed,"	// row[3]
		          "Stem"		// row[4]
		    " FROM svy_questions"
		   " WHERE QstCod=%ld"
		     " AND SvyCod=%ld",	// Extra check
		   QstCod,
		   SvyCod);
   }

/*****************************************************************************/
/************ Get question index from question code in a survey **************/
/*****************************************************************************/

unsigned Svy_DB_GetQstIndFromQstCod (long QstCod)
  {
   return DB_QuerySELECTUnsigned ("can not get question index",
				  "SELECT QstInd"
				   " FROM svy_questions"
				  " WHERE QstCod=%ld",
				  QstCod);
  }

/*****************************************************************************/
/*********************** Get last question index *****************************/
/*****************************************************************************/

unsigned Svy_DB_GetLastQstInd (MYSQL_RES **mysql_res,long SvyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get last question index",
		   "SELECT MAX(QstInd)"		// row[0]
		    " FROM svy_questions"
		   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Svy_DB_RemoveQst (long QstCod)
  {
   DB_QueryDELETE ("can not remove a question",
		   "DELETE FROM svy_questions"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/********************* Remove all questions in a survey **********************/
/*****************************************************************************/

void Svy_DB_RemoveQstsSvy (long SvyCod)
  {
   DB_QueryDELETE ("can not remove questions of a survey",
		   "DELETE FROM svy_questions"
                   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/******** Remove questions in all surveys of a place on the hierarchy ********/
/******** (country, institution, center, degree or course)            ********/
/*****************************************************************************/

void Svy_DB_RemoveQstsSvysIn (Hie_Level_t HieLvl,long HieCod)
  {
   DB_QueryDELETE ("can not remove questions of surveys"
		   " in a place on the hierarchy",
		   "DELETE FROM svy_questions"
	           " USING svy_surveys,"
	                  "svy_questions"
                   " WHERE svy_surveys.Scope='%s'"
                     " AND svy_surveys.Cod=%ld"
                     " AND svy_surveys.SvyCod=svy_questions.SvyCod",
		   Hie_GetDBStrFromLevel (HieLvl),
		   HieCod);
  }

/*****************************************************************************/
/******************************* Create answer *******************************/
/*****************************************************************************/

void Svy_DB_CreateAnswer (long QstCod,unsigned AnsInd,const char *Text)
  {
   DB_QueryINSERT ("can not create answer",
		   "INSERT INTO svy_answers"
		   " (QstCod,AnsInd,NumUsrs,Answer)"
		   " VALUES"
		   " (%ld,%u,0,'%s')",
		   QstCod,
		   AnsInd,
		   Text);
  }

/*****************************************************************************/
/**************************** Update answer text *****************************/
/*****************************************************************************/

void Svy_DB_UpdateAnswerText (long QstCod,unsigned AnsInd,const char *Text)
  {
   DB_QueryUPDATE ("can not update answer",
		   "UPDATE svy_answers"
		     " SET Answer='%s'"
		   " WHERE QstCod=%ld"
		     " AND AnsInd=%u",
		   Text,
		   QstCod,
		   AnsInd);
  }

/*****************************************************************************/
/********************** Reset all answers in a survey ************************/
/*****************************************************************************/

void Svy_DB_ResetAnswersSvy (long SvyCod)
  {
   DB_QueryUPDATE ("can not reset answers of a survey",
		   "UPDATE svy_answers,"
		          "svy_questions"
		     " SET svy_answers.NumUsrs=0"
                   " WHERE svy_questions.SvyCod=%ld"
                     " AND svy_questions.QstCod=svy_answers.QstCod",
		   SvyCod);
  }

/*****************************************************************************/
/************ Increase number of users who have marked one answer ************/
/*****************************************************************************/

void Svy_DB_IncreaseAnswer (long QstCod,unsigned AnsInd)
  {
   /***** Increase number of users who have selected
          the answer AnsInd in the question QstCod *****/
   DB_QueryUPDATE ("can not register your answer to the survey",
		   "UPDATE svy_answers"
		     " SET NumUsrs=NumUsrs+1"
                   " WHERE QstCod=%ld"
                     " AND AnsInd=%u",
		   QstCod,
		   AnsInd);
  }

/*****************************************************************************/
/*********** Check if an answer of a question exists in database *************/
/*****************************************************************************/

bool Svy_DB_CheckIfAnswerExists (long QstCod,unsigned AnsInd)
  {
   return
   DB_QueryEXISTS ("can not check if an answer exists",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM svy_answers"
		    " WHERE QstCod=%ld"
		      " AND AnsInd=%u)",
		   QstCod,
		   AnsInd);
  }

/*****************************************************************************/
/************** Get answers of a survey question from database ***************/
/*****************************************************************************/

unsigned Svy_DB_GetAnswersQst (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get answers of a question",
		   "SELECT AnsInd,"	// row[0]
			  "NumUsrs,"	// row[1]
			  "Answer"	// row[2]
		    " FROM svy_answers"
		   " WHERE QstCod=%ld"
		" ORDER BY AnsInd",
		   QstCod);
  }

/*****************************************************************************/
/*********** Convert a string with the answer type to answer type ************/
/*****************************************************************************/

Svy_AnswerType_t Svy_DB_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeDB)
  {
   Svy_AnswerType_t AnsType;

   for (AnsType  = (Svy_AnswerType_t) 0;
	AnsType <= (Svy_AnswerType_t) (Svy_NUM_ANS_TYPES - 1);
	AnsType++)
      if (!strcmp (StrAnsTypeDB,Svy_DB_StrAnswerTypes[AnsType]))
         return AnsType;

   return (Svy_AnswerType_t) 0;
  }

/*****************************************************************************/
/********************** Remove an answer from a question *********************/
/*****************************************************************************/

void Svy_DB_RemoveAnswerQst (long QstCod,unsigned AnsInd)
  {
   DB_QueryDELETE ("can not delete answer",
		   "DELETE FROM svy_answers"
		   " WHERE QstCod=%ld"
		     " AND AnsInd=%u",
		   QstCod,
		   AnsInd);
  }

/*****************************************************************************/
/********************* Remove answers of a survey question *******************/
/*****************************************************************************/

void Svy_DB_RemoveAnswersQst (long QstCod)
  {
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM svy_answers"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/*********************** Remove all answers in a survey **********************/
/*****************************************************************************/

void Svy_DB_RemoveAnswersSvy (long SvyCod)
  {
   DB_QueryDELETE ("can not remove answers of a survey",
		   "DELETE FROM svy_answers"
		   " USING svy_questions,"
		          "svy_answers"
                   " WHERE svy_questions.SvyCod=%ld"
                   " AND svy_questions.QstCod=svy_answers.QstCod",
		   SvyCod);
  }

/*****************************************************************************/
/********* Remove answers to all surveys of a place on the hierarchy *********/
/********* (country, institution, center, degree or course)          *********/
/*****************************************************************************/

void Svy_DB_RemoveAnswersSvysIn (Hie_Level_t HieLvl,long HieCod)
  {
   DB_QueryDELETE ("can not remove answers of surveys"
		   " in a place on the hierarchy",
		   "DELETE FROM svy_answers"
	           " USING svy_surveys,"
	                  "svy_questions,"
	                  "svy_answers"
                   " WHERE svy_surveys.Scope='%s'"
                     " AND svy_surveys.Cod=%ld"
                     " AND svy_surveys.SvyCod=svy_questions.SvyCod"
                     " AND svy_questions.QstCod=svy_answers.QstCod",
		   Hie_GetDBStrFromLevel (HieLvl),
		   HieCod);
  }

/*****************************************************************************/
/************** Create new comments for a given survey question **************/
/*****************************************************************************/

void Svy_DB_CreateComments (long QstCod,const char *Comments)
  {
   DB_QueryINSERT ("can not create comments",
		   "INSERT INTO svy_comments"
		   " (QstCod,Comments)"
		   " VALUES"
		   " (%ld,'%s')",
                   QstCod,
                   Comments);
  }

/*****************************************************************************/
/************* Get comments of a survey question from database ***************/
/*****************************************************************************/

unsigned Svy_DB_GetCommentsQst (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments of a question",
		   "SELECT Comments"	// row[0]
		    " FROM svy_comments"
		   " WHERE QstCod=%ld"
		" ORDER BY ComCod",
		   QstCod);
  }

/*****************************************************************************/
/********************* Remove comments of a survey question ******************/
/*****************************************************************************/

void Svy_DB_RemoveCommentsQst (long QstCod)
  {
   DB_QueryDELETE ("can not remove the comments of a question",
		   "DELETE FROM svy_comments"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/*********************** Remove all comments in a survey *********************/
/*****************************************************************************/

void Svy_DB_RemoveCommentsSvy (long SvyCod)
  {
   DB_QueryDELETE ("can not remove comments of a survey",
		   "DELETE FROM svy_comments"
		   " USING svy_questions,"
		          "svy_comments"
                   " WHERE svy_questions.SvyCod=%ld"
                   " AND svy_questions.QstCod=svy_comments.QstCod",
		   SvyCod);
  }

/*****************************************************************************/
/********* Remove comments to all surveys of a place on the hierarchy ********/
/********* (country, institution, center, degree or course)           ********/
/*****************************************************************************/

void Svy_DB_RemoveCommentsSvysIn (Hie_Level_t HieLvl,long HieCod)
  {
   DB_QueryDELETE ("can not remove comments of surveys"
		   " in a place on the hierarchy",
		   "DELETE FROM svy_comments"
	           " USING svy_surveys,"
	                  "svy_questions,"
	                  "svy_comments"
                   " WHERE svy_surveys.Scope='%s'"
                     " AND svy_surveys.Cod=%ld"
                     " AND svy_surveys.SvyCod=svy_questions.SvyCod"
                     " AND svy_questions.QstCod=svy_comments.QstCod",
		   Hie_GetDBStrFromLevel (HieLvl),
		   HieCod);
  }

/*****************************************************************************/
/***************** Register that I have answered this survey *****************/
/*****************************************************************************/

void Svy_DB_RegisterIHaveAnsweredSvy (long SvyCod)
  {
   DB_QueryINSERT ("can not register that you have answered the survey",
		   "INSERT INTO svy_users"
	           " (SvyCod,UsrCod)"
                   " VALUES"
                   " (%ld,%ld)",
		   SvyCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/**************** Check if I have answered a given survey ********************/
/*****************************************************************************/

bool Svy_DB_CheckIfIHaveAnsweredSvy (long SvyCod)
  {
   return
   DB_QueryEXISTS ("can not check if you have answered a survey",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM svy_users"
		    " WHERE SvyCod=%ld"
		      " AND UsrCod=%ld)",
		   SvyCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Get number of users who have answered a survey ***************/
/*****************************************************************************/

unsigned Svy_DB_GetNumUsrsWhoHaveAnsweredSvy (long SvyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users who have answered a survey",
		  "SELECT COUNT(*)"
		   " FROM svy_users"
		  " WHERE SvyCod=%ld",
		  SvyCod);
  }

/*****************************************************************************/
/********************** Remove all users in this survey **********************/
/*****************************************************************************/

void Svy_DB_RemoveUsrsWhoHaveAnsweredSvy (long SvyCod)
  {
   DB_QueryDELETE ("can not remove users who are answered a survey",
		   "DELETE FROM svy_users"
		   " WHERE SvyCod=%ld",
		   SvyCod);
  }

/*****************************************************************************/
/************* Remove all surveys in a place on the hierarchy   **************/
/************* (country, institution, center, degree or course) **************/
/*****************************************************************************/

void Svy_DB_RemoveUsrsWhoHaveAnsweredSvysIn (Hie_Level_t HieLvl,long HieCod)
  {
   DB_QueryDELETE ("can not remove users"
	           " who had answered surveys in a place on the hierarchy",
		   "DELETE FROM svy_users"
	           " USING svy_surveys,"
	                  "svy_users"
                   " WHERE svy_surveys.Scope='%s'"
                     " AND svy_surveys.Cod=%ld"
                     " AND svy_surveys.SvyCod=svy_users.SvyCod",
		   Hie_GetDBStrFromLevel (HieLvl),
		   HieCod);
  }
