// swad_rubric_database.c: assessment rubrics, operations with database

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

#include <stdlib.h>		// For free

#include "swad_database.h"
#include "swad_error.h"
#include "swad_rubric_database.h"
#include "swad_rubric_criteria.h"
#include "swad_global.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

#define RubCri_AFTER_LAST_CRITERION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database

// Fields in database for minimum/maximum criterion values
static const char *RubCri_ValuesFields[RubCri_NUM_VALUES] =
  {
   [RubCri_MIN] = "MinVal",
   [RubCri_MAX] = "MaxVal",
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Create a new rubric *****************************/
/*****************************************************************************/

long Rub_DB_CreateRubric (const struct Rub_Rubric *Rubric)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new rubric",
				"INSERT INTO rub_rubrics"
				" (CrsCod,UsrCod,Title,Txt)"
				" VALUES"
				" (%ld,%ld,'%s','%s')",
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Rubric->Title,
				Rubric->Txt);
  }

/*****************************************************************************/
/************************** Update an existing rubric ************************/
/*****************************************************************************/

void Rub_DB_UpdateRubric (const struct Rub_Rubric *Rubric)
  {
   DB_QueryUPDATE ("can not update rubric",
		   "UPDATE rub_rubrics"
		     " SET CrsCod=%ld,"
		          "Title='%s',"
		          "Txt='%s'"
		   " WHERE RubCod=%ld",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	           Rubric->Title,
	           Rubric->Txt,
	           Rubric->RubCod);
  }

/*****************************************************************************/
/************ Get list of all rubric codes in the current course *************/
/*****************************************************************************/

unsigned Rub_DB_GetListRubrics (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get rubrics",
		   "SELECT RubCod"		// row[0]
		    " FROM rub_rubrics"
		   " WHERE CrsCod=%ld"
		" ORDER BY Title",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************* Get rubric data using its code ************************/
/*****************************************************************************/

Exi_Exist_t Rub_DB_GetRubricDataByCod (MYSQL_RES **mysql_res,long RubCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get rubric data",
			 "SELECT RubCod,"	// row[0]
				"CrsCod,"	// row[1]
				"UsrCod,"	// row[2]
				"Title"		// row[3]
			  " FROM rub_rubrics"
			 " WHERE RubCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
			 RubCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************** Get rubric title from database ***********************/
/*****************************************************************************/

void Rub_DB_GetRubricTitle (long RubCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get rubric title",
		         "SELECT Title"	// row[0]
			  " FROM rub_rubrics"
		         " WHERE RubCod=%ld"
		           " AND CrsCod=%ld",	// Extra check
			 RubCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************** Get rubric text from database ************************/
/*****************************************************************************/
// Rubric->Txt must be allocated with Cns_MAX_BYTES_TEXT + 1 bytes

void Rub_DB_GetRubricTxt (struct Rub_Rubric *Rubric)
  {
   DB_QuerySELECTString (Rubric->Txt,Cns_MAX_BYTES_TEXT,"can not get rubric text",
		         "SELECT Txt"	// row[0]
			  " FROM rub_rubrics"
		         " WHERE RubCod=%ld",
		         Rubric->RubCod);
  }

/*****************************************************************************/
/******************* Check if the title of a rubric exists *******************/
/*****************************************************************************/

Exi_Exist_t Rub_DB_CheckIfSimilarRubricExists (const struct Rub_Rubric *Rubric)
  {
   return
   DB_QueryEXISTS ("can not check similar rubrics",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM rub_rubrics"
		    " WHERE CrsCod=%ld"
		      " AND Title='%s'"
		      " AND RubCod<>%ld)",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Rubric->Title,
		   Rubric->RubCod);
  }

/*****************************************************************************/
/******************** Get number of courses with rubrics *********************/
/*****************************************************************************/
// Returns the number of courses with rubrics in this location

unsigned Rub_DB_GetNumCoursesWithRubrics (Hie_Level_t HieLvl)
  {
   /***** Get number of courses with rubrics from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with rubrics",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM rub_rubrics");
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with rubrics",
			 "SELECT COUNT(DISTINCT rub_rubrics.CrsCod)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "rub_rubrics"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
                         Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with rubrics",
			 "SELECT COUNT(DISTINCT rub_rubrics.CrsCod)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "rub_rubrics"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
		         Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with rubrics",
			 "SELECT COUNT(DISTINCT rub_rubrics.CrsCod)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "rub_rubrics"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
                         Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with rubrics",
			 "SELECT COUNT(DISTINCT rub_rubrics.CrsCod)"
			  " FROM crs_courses,"
			        "rub_rubrics"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
		         Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with rubrics",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM rub_rubrics"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/*************************** Get number of rubrics ***************************/
/*****************************************************************************/
// Returns the number of rubrics in a given location

unsigned Rub_DB_GetNumRubrics (Hie_Level_t HieLvl)
  {
   /***** Get number of rubrics from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of rubrics",
                         "SELECT COUNT(*)"
			  " FROM rub_rubrics");
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of rubrics",
                         "SELECT COUNT(*)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "rub_rubrics"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
		         Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of rubrics",
                         "SELECT COUNT(*)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "rub_rubrics"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
		         Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of rubrics",
                         "SELECT COUNT(*)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "rub_rubrics"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
		         Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of rubrics",
                         "SELECT COUNT(*)"
			  " FROM crs_courses,"
			        "rub_rubrics"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=rub_rubrics.CrsCod",
		         Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of rubrics",
                         "SELECT COUNT(*)"
			  " FROM rub_rubrics"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/******************************** Remove rubric ******************************/
/*****************************************************************************/

void Rub_DB_RemoveRubric (long RubCod)
  {
   DB_QueryDELETE ("can not remove rubric",
		   "DELETE FROM rub_rubrics"
		   " WHERE RubCod=%ld",
		   RubCod);
  }

/*****************************************************************************/
/********************** Remove the rubrics of a course ***********************/
/*****************************************************************************/

void Rub_DB_RemoveCrsRubrics (long HieCod)
  {
   DB_QueryDELETE ("can not remove course rubrics",
		   "DELETE FROM rub_rubrics"
		   " WHERE CrsCod=%ld",
                   HieCod);
  }

/*****************************************************************************/
/********************** Create a new rubric criterion ************************/
/*****************************************************************************/

long Rub_DB_CreateCriterion (const struct RubCri_Criterion *Criterion)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];
   long CriCod;

   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   CriCod =
   DB_QueryINSERTandReturnCode ("can not create new criterion",
				"INSERT INTO rub_criteria"
				" (RubCod,CriInd,Type,Cod,%s,%s,Weight,Title)"
				" VALUES"
				" (%ld,%u,'%s',%ld,%.15lg,%.15lg,%.15lg,'%s')",
				RubCri_ValuesFields[RubCri_MIN],
				RubCri_ValuesFields[RubCri_MAX],
				Criterion->RubCod,
				Criterion->CriInd,
				Rsc_DB_Types[Criterion->Link.Type],
				Criterion->Link.Cod,
				Criterion->Values[RubCri_MIN],
				Criterion->Values[RubCri_MAX],
				Criterion->Weight,
				Criterion->Title);
   Str_SetDecimalPointToLocal ();	// Return to local system

   return CriCod;
  }

/*****************************************************************************/
/********************* Update criterion title in database ********************/
/*****************************************************************************/

void Rub_DB_UpdateCriterionTitle (const struct RubCri_Criterion *Criterion)
  {
   DB_QueryUPDATE ("can not update the title of a criterion",
		   "UPDATE rub_criteria"
		     " SET Title='%s'"
		   " WHERE CriCod=%ld"
		     " AND RubCod=%ld",	// Extra check
	           Criterion->Title,
	           Criterion->CriCod,
	           Criterion->RubCod);
  }

/*****************************************************************************/
/******** Update the link to a resource in a criterion given its code ********/
/*****************************************************************************/

void Rub_DB_UpdateCriterionLink (const struct RubCri_Criterion *Criterion)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];

   DB_QueryUPDATE ("can not update link of criterion",
		   "UPDATE rub_criteria"
		     " SET Type='%s',"
		          "Cod=%ld"
		   " WHERE CriCod=%ld",
		   Rsc_DB_Types[Criterion->Link.Type],
		   Criterion->Link.Cod,
		   Criterion->CriCod);
  }

/*****************************************************************************/
/********************* Update criterion value in database ********************/
/*****************************************************************************/

void Rub_DB_UpdateCriterionValue (const struct RubCri_Criterion *Criterion,
                                  RubCri_ValueRange_t ValueRange)
  {
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   DB_QueryUPDATE ("can not update the value of a criterion",
		   "UPDATE rub_criteria"
		     " SET %s=%.15lg"
		   " WHERE CriCod=%ld"
		     " AND RubCod=%ld",	// Extra check
	           RubCri_ValuesFields[ValueRange],Criterion->Values[ValueRange],
	           Criterion->CriCod,
	           Criterion->RubCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/********************* Update criterion weight in database *******************/
/*****************************************************************************/

void Rub_DB_UpdateCriterionWeight (const struct RubCri_Criterion *Criterion)
  {
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   DB_QueryUPDATE ("can not update the value of a criterion",
		   "UPDATE rub_criteria"
		     " SET Weight=%.15lg"
		   " WHERE CriCod=%ld"
		     " AND RubCod=%ld",	// Extra check
	           Criterion->Weight,
	           Criterion->CriCod,
	           Criterion->RubCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************ Update indexes of criteria greater than a given one ************/
/*****************************************************************************/

void Rub_DB_UpdateCriteriaIndexesInRubricGreaterThan (const struct RubCri_Criterion *Criterion)
  {
   DB_QueryUPDATE ("can not update indexes of criteria",
		   "UPDATE rub_criteria"
		     " SET CriInd=CriInd-1"
		   " WHERE RubCod=%ld"
		     " AND CriInd>%u",
		   Criterion->RubCod,
		   Criterion->CriInd);
  }

/*****************************************************************************/
/****************** Change index of a criterion in a rubric ******************/
/*****************************************************************************/

void Rub_DB_UpdateCriterionIndex (long CriInd,long CriCod,long RubCod)
  {
   DB_QueryUPDATE ("can not exchange indexes of criteria",
		   "UPDATE rub_criteria"
		     " SET CriInd=%ld"
		   " WHERE CriCod=%ld"
		     " AND RubCod=%ld",	// Extra check
		   CriInd,
		   CriCod,
		   RubCod);
  }

/*****************************************************************************/
/************ Lock table to make the exchange of criteria atomic ***********/
/*****************************************************************************/

void Rub_DB_LockTable (void)
  {
   DB_Query ("can not lock tables to move rubric criterion",
	     "LOCK TABLES rub_criteria WRITE");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/******************* Get number of criteria of a rubric *********************/
/*****************************************************************************/

unsigned Rub_DB_GetNumCriteriaInRubric (long RubCod)
  {
   /***** Get nuumber of criteria in a rubric from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of criteria of a rubric",
		  "SELECT COUNT(*)"
		   " FROM rub_criteria"
		  " WHERE RubCod=%ld",
		  RubCod);
  }

/*****************************************************************************/
/****************** Get rubric code given a criterion code *******************/
/*****************************************************************************/
// Return <0 is criterion is not present in any rubric

long Rub_DB_GetRubCodFromCriCod (long CriCod)
  {
   return DB_QuerySELECTCode ("can not get which rubric",
			      "SELECT RubCod"	// row[0]
			       " FROM rub_criteria"
			      " WHERE CriCod=%ld",
			      CriCod);
  }

/*****************************************************************************/
/********* Get criterion code given rubric and index of criterion ************/
/*****************************************************************************/

long Rub_DB_GetCriCodFromCriInd (long RubCod,unsigned CriInd)
  {
   long CriCod;

   CriCod = DB_QuerySELECTCode ("can not get criterion code",
				"SELECT CriCod"
				 " FROM rub_criteria"
				" WHERE RubCod=%ld"
				  " AND CriInd=%u",
				RubCod,
				CriInd);
   if (CriCod <= 0)
      Err_WrongCriterionExit ();

   return CriCod;
  }

/*****************************************************************************/
/********** Get criterion index given rubric and code of criterion ***********/
/*****************************************************************************/
// Return 0 is criterion is not present in rubric

unsigned Rub_DB_GetCriIndFromCriCod (long RubCod,long CriCod)
  {
   return DB_QuerySELECTUnsigned ("can not get criterion index",
				  "SELECT CriInd"
				   " FROM rub_criteria"
				  " WHERE RubCod=%ld"
				    " AND CriCod=%ld",
				  RubCod,
				  CriCod);
  }

/*****************************************************************************/
/**************** Get maximum criterion index in a rubric ********************/
/*****************************************************************************/
// Criterion index can be 1, 2, 3...
// Return 0 if no criteria

unsigned Rub_DB_GetMaxCriterionIndexInRubric (long RubCod)
  {
   /***** Get maximum criterion index in a rubric from database *****/
   return DB_QuerySELECTUnsigned ("can not get last criterion index",
				  "SELECT MAX(CriInd)"
				   " FROM rub_criteria"
				  " WHERE RubCod=%ld",
				  RubCod);
  }

/*****************************************************************************/
/********* Get previous criterion index to a given index in a rubric *********/
/*****************************************************************************/
// Input criterion index can be 1, 2, 3... n-1
// Return criterion index will be 1, 2, 3... n if previous criterion exists, or 0 if no previous criterion

unsigned Rub_DB_GetPrevCriterionIndexInRubric (long RubCod,unsigned CriInd)
  {
   /***** Get previous criterion index in a rubric from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   return DB_QuerySELECTUnsigned ("can not get previous criterion index",
				  "SELECT COALESCE(MAX(CriInd),0)"
				   " FROM rub_criteria"
				  " WHERE RubCod=%ld"
				    " AND CriInd<%u",
				  RubCod,
				  CriInd);
  }

/*****************************************************************************/
/*********** Get next criterion index to a given index in a rubric ***********/
/*****************************************************************************/
// Input criterion index can be 0, 1, 2, 3... n-1
// Return criterion index will be 1, 2, 3... n if next criterion exists, or big number if no next criterion

unsigned Rub_DB_GetNextCriterionIndexInRubric (long RubCod,unsigned CriInd)
  {
   /***** Get next criterion index in a rubric from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   return DB_QuerySELECTUnsigned ("can not get next criterion index",
				  "SELECT COALESCE(MIN(CriInd),%u)"
				   " FROM rub_criteria"
				  " WHERE RubCod=%ld"
				    " AND CriInd>%u",
				  RubCri_AFTER_LAST_CRITERION,	// End of criteria has been reached
				  RubCod,
				  CriInd);
  }

/*****************************************************************************/
/********************* Get criteria in a given rubric ************************/
/*****************************************************************************/

unsigned Rub_DB_GetCriteria (MYSQL_RES **mysql_res,long RubCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get criteria",
		   "SELECT CriCod,"	// row[0]
			  "RubCod,"	// row[1]
			  "CriInd,"	// row[2]
                          "Type,"	// row[3]
                          "Cod,"	// row[4]
			  "%s,"		// row[5]
			  "%s,"		// row[6]
                          "Weight,"	// row[7]
			  "Title"	// row[8]
		    " FROM rub_criteria"
		   " WHERE RubCod=%ld"
		" ORDER BY CriInd",
		   RubCri_ValuesFields[RubCri_MIN],
		   RubCri_ValuesFields[RubCri_MAX],
		   RubCod);
  }

/*****************************************************************************/
/***************** Get rubric criterion data using its code ******************/
/*****************************************************************************/

Exi_Exist_t Rub_DB_GetCriterionDataByCod (MYSQL_RES **mysql_res,long CriCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get criterion data",
			 "SELECT CriCod,"	// row[0]
				"RubCod,"	// row[1]
				"CriInd,"	// row[2]
				"Type,"		// row[3]
				"Cod,"		// row[4]
				"%s,"		// row[5]
				"%s,"		// row[6]
				"Weight,"	// row[7]
				"Title"		// row[8]
			  " FROM rub_criteria"
			 " WHERE CriCod=%ld",
			 RubCri_ValuesFields[RubCri_MIN],
			 RubCri_ValuesFields[RubCri_MAX],
			 CriCod);
  }

/*****************************************************************************/
/************** Check if the title of a rubric criterion exists **************/
/*****************************************************************************/

Exi_Exist_t Rub_DB_CheckIfSimilarCriterionExists (const struct RubCri_Criterion *Criterion,
						  const char Title[RubCri_MAX_BYTES_TITLE + 1])
  {
   return
   DB_QueryEXISTS ("can not check similar criterion",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM rub_criteria,"
			   "rub_rubrics"
		    " WHERE rub_criteria.RubCod=%ld"
		      " AND rub_criteria.Title='%s'"
		      " AND rub_criteria.CriCod<>%ld"
		      " AND rub_criteria.RubCod=rub_rubrics.RubCod"
		      " AND rub_rubrics.CrsCod=%ld)",	// Extra check
		   Criterion->RubCod,
		   Title,
		   Criterion->CriCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**************** Get average number of criteria per rubric ******************/
/*****************************************************************************/

double Rub_DB_GetNumCriteriaPerRubric (Hie_Level_t HieLvl)
  {
   /***** Get number of criteria per rubric from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
         return
         DB_QuerySELECTDouble ("can not get number of criteria per rubric",
			       "SELECT AVG(NumCriteria)"
			        " FROM (SELECT COUNT(rub_criteria.CriCod) AS NumCriteria"
				        " FROM rub_rubrics,"
					      "rub_criteria"
				       " WHERE rub_rubrics.RubCod=rub_criteria.RubCod"
				    " GROUP BY rub_criteria.RubCod) AS NumCriteriaTable");
      case Hie_CTY:
         return
         DB_QuerySELECTDouble ("can not get number of criteria per rubric",
			       "SELECT AVG(NumCriteria)"
			       " FROM (SELECT COUNT(rub_criteria.CriCod) AS NumCriteria"
				       " FROM ins_instits,"
					     "ctr_centers,"
					     "deg_degrees,"
					     "crs_courses,"
					     "rub_rubrics,"
					     "rub_criteria"
				      " WHERE ins_instits.CtyCod=%ld"
				        " AND ins_instits.InsCod=ctr_centers.InsCod"
				        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				        " AND deg_degrees.DegCod=crs_courses.DegCod"
				        " AND crs_courses.CrsCod=rub_rubrics.CrsCod"
				        " AND rub_rubrics.RubCod=rub_criteria.RubCod"
				   " GROUP BY rub_criteria.RubCod) AS NumCriteriaTable",
			       Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return
         DB_QuerySELECTDouble ("can not get number of criteria per rubric",
			       "SELECT AVG(NumCriteria)"
			       " FROM (SELECT COUNT(rub_criteria.CriCod) AS NumCriteria"
				       " FROM ctr_centers,"
					     "deg_degrees,"
					     "crs_courses,"
					     "rub_rubrics,"
					     "rub_criteria"
				      " WHERE ctr_centers.InsCod=%ld"
				        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				        " AND deg_degrees.DegCod=crs_courses.DegCod"
				        " AND crs_courses.CrsCod=rub_rubrics.CrsCod"
				        " AND rub_rubrics.RubCod=rub_criteria.RubCod"
				   " GROUP BY rub_criteria.RubCod) AS NumCriteriaTable",
			       Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return
         DB_QuerySELECTDouble ("can not get number of criteria per rubric",
			       "SELECT AVG(NumCriteria)"
			        " FROM (SELECT COUNT(rub_criteria.CriCod) AS NumCriteria"
				        " FROM deg_degrees,"
					      "crs_courses,"
					      "rub_rubrics,"
					      "rub_criteria"
				       " WHERE deg_degrees.CtrCod=%ld"
				         " AND deg_degrees.DegCod=crs_courses.DegCod"
				         " AND crs_courses.CrsCod=rub_rubrics.CrsCod"
				         " AND rub_rubrics.RubCod=rub_criteria.RubCod"
				    " GROUP BY rub_criteria.RubCod) AS NumCriteriaTable",
			       Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return
         DB_QuerySELECTDouble ("can not get number of criteria per rubric",
			       "SELECT AVG(NumCriteria)"
			        " FROM (SELECT COUNT(rub_criteria.CriCod) AS NumCriteria"
				        " FROM crs_courses,"
					      "rub_rubrics,"
					      "rub_criteria"
				       " WHERE crs_courses.DegCod=%ld"
				         " AND crs_courses.CrsCod=rub_rubrics.CrsCod"
				         " AND rub_rubrics.RubCod=rub_criteria.RubCod"
				    " GROUP BY rub_criteria.RubCod) AS NumCriteriaTable",
			       Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return
         DB_QuerySELECTDouble ("can not get number of criteria per rubric",
			       "SELECT AVG(NumCriteria)"
			        " FROM (SELECT COUNT(rub_criteria.CriCod) AS NumCriteria"
				        " FROM rub_rubrics,"
					      "rub_criteria"
				       " WHERE rub_rubrics.CrsCod=%ld"
				         " AND rub_rubrics.RubCod=rub_criteria.RubCod"
				    " GROUP BY rub_criteria.RubCod) AS NumCriteriaTable",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0.0;	// Not reached
     }
  }

/*****************************************************************************/
/*********************** Remove criterion from a rubric **********************/
/*****************************************************************************/

void Rub_DB_RemoveCriterionFromRubric (const struct RubCri_Criterion *Criterion)
  {
   DB_QueryDELETE ("can not remove rubric criterion",
		   "DELETE FROM rub_criteria"
		   " WHERE CriCod=%ld"
                     " AND RubCod=%ld",	// Extra check
		   Criterion->CriCod,
		   Criterion->RubCod);
  }

/*****************************************************************************/
/**************************** Remove rubric criteria *************************/
/*****************************************************************************/

void Rub_DB_RemoveRubricCriteria (long RubCod)
  {
   DB_QueryDELETE ("can not remove rubric criteria",
		   "DELETE FROM rub_criteria"
		   " WHERE RubCod=%ld",
		   RubCod);
  }

/*****************************************************************************/
/**************** Remove the criteria in rubrics of a course *****************/
/*****************************************************************************/

void Rub_DB_RemoveCrsRubricCriteria (long HieCod)
  {
   DB_QueryDELETE ("can not remove criteria in course rubrics",
		   "DELETE FROM rub_criteria"
		   " USING rub_rubrics,"
		          "rub_criteria"
		   " WHERE rub_rubrics.CrsCod=%ld"
		     " AND rub_rubrics.RubCod=rub_criteria.RubCod",
                   HieCod);
  }

/*****************************************************************************/
/** Update score of a criterion given an assignment/project code and a user **/
/*****************************************************************************/

void Rub_DB_UpdateScore (Rsc_Type_t RscType,long RscCod,long UsrCod,long CriCod,
                         double Score)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];

   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   DB_QueryREPLACE ("can not save score",
		    "REPLACE INTO rub_scores"
                    " (Type,Cod,UsrCod,CriCod,EvlCod,Score)"
                    " VALUES"
                    " ('%s',%ld,%ld,%ld,%ld,%.15lg)",
                    Rsc_DB_Types[RscType],RscCod,UsrCod,CriCod,
                    Gbl.Usrs.Me.UsrDat.UsrCod,Score);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/**** Get score of a criterion given an assignment/project code and a user ***/
/*****************************************************************************/

double Rub_DB_GetScore (Rsc_Type_t RscType,long RscCod,long UsrCod,long CriCod)
  {
   extern const char *Rsc_DB_Types[Rsc_NUM_TYPES];

   return DB_QuerySELECTDouble ("can not get score",
				"SELECT Score"
				 " FROM rub_scores"
				" WHERE Type='%s'"
				  " AND Cod=%ld"
                                  " AND UsrCod=%ld"
				  " AND CriCod=%ld",
				Rsc_DB_Types[RscType],
				RscCod,
				UsrCod,
				CriCod);
  }
