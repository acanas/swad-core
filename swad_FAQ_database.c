// swad_FAQ_database.c: Frequently Asked Questions, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_FAQ.h"
#include "swad_FAQ_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*********************** Create a new question & answer ************************/
/*****************************************************************************/

long FAQ_DB_CreateQaA (const struct Tre_Node *Node)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new question-answer",
				"INSERT INTO faq_questions"
				" (NodCod,ItmInd,Hidden,Question,Answer)"
				" SELECT %ld,COALESCE(MAX(t2.ItmInd),0)+1,'N','%s','%s'"
				  " FROM faq_questions AS t2"
				 " WHERE t2.NodCod=%ld",
				Node->Hierarchy.NodCod,
				Node->QaA.Question,
				Node->QaA.Answer,
				Node->Hierarchy.NodCod);
  }

/*****************************************************************************/
/************* Get list of node questions & answers from database **************/
/*****************************************************************************/

unsigned FAQ_DB_GetListQaAs (MYSQL_RES **mysql_res,long NodCod,
                             bool ShowHiddenQaAs)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   static const char *HiddenSubQuery[2] =
     {
      [false] = " AND faq_questions.Hidden='N'",
      [true ] = "",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node questions & answers",
		   "SELECT faq_questions.NodCod,"	// row[0]
			  "faq_questions.QaACod,"	// row[1]
                          "faq_questions.ItmInd,"	// row[2]
			  "faq_questions.Hidden,"	// row[3]
			  "faq_questions.Question,"	// row[4]
			  "faq_questions.Answer"	// row[5]
		    " FROM faq_questions,"
		          "tre_nodes"
		   " WHERE faq_questions.NodCod=%ld"
		       "%s"
		     " AND faq_questions.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'"		// Extra check
		" ORDER BY faq_questions.ItmInd",
		   NodCod,
		   HiddenSubQuery[ShowHiddenQaAs],
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_FAQ]);
  }

/*****************************************************************************/
/**************** Get question & answer data using its code ********************/
/*****************************************************************************/

unsigned FAQ_DB_GetQaADataByCod (MYSQL_RES **mysql_res,long QaACod)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get node question & answer data",
		   "SELECT faq_questions.NodCod,"	// row[0]
			  "faq_questions.QaACod,"	// row[1]
                          "faq_questions.ItmInd,"	// row[2]
			  "faq_questions.Hidden,"	// row[3]
			  "faq_questions.Question,"	// row[4]
			  "faq_questions.Answer"	// row[5]
		    " FROM faq_questions,"
		          "tre_nodes"
		   " WHERE faq_questions.QaACod=%ld"
		     " AND faq_questions.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   QaACod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_FAQ]);
  }

/*****************************************************************************/
/**** Get question & answer code given node code and question & answer index *****/
/*****************************************************************************/

long FAQ_DB_GetQaACodFromQaAInd (long NodCod,unsigned ItmInd)
  {
   /***** Trivial check: question & answer index should be > 0 *****/
   if (ItmInd == 0)
      return -1L;

   /***** Get question & answer code given node code and question & answer index *****/
   return DB_QuerySELECTCode ("can not get question & answer code",
			      "SELECT QaACod"
			       " FROM faq_questions"
			      " WHERE NodCod=%ld"
				" AND ItmInd=%u",
			      NodCod,ItmInd);
  }

/*****************************************************************************/
/*********************** Remove a node question & answer ***********************/
/*****************************************************************************/

void FAQ_DB_RemoveQaA (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryDELETE ("can not remove node question-answer",
		   "DELETE FROM faq_questions"
		   " USING faq_questions,"
		          "tre_nodes"
		   " WHERE faq_questions.QaACod=%ld"
		     " AND faq_questions.NodCod=%ld"
                     " AND faq_questions.NodCod=tre_nodes.NodCod"
                     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_FAQ]);
  }

/*****************************************************************************/
/********************* Hide/unhide a node question & answer ********************/
/*****************************************************************************/

void FAQ_DB_HideOrUnhideQaA (const struct Tre_Node *Node,
			     HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not hide/unhide node question-answer",
		   "UPDATE faq_questions,"
		          "tre_nodes"
		     " SET faq_questions.Hidden='%c'"
		   " WHERE faq_questions.QaACod=%ld"
		     " AND faq_questions.NodCod=%ld"
		     " AND faq_questions.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   HidVis_YN[HiddenOrVisible],
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_FAQ]);
  }

/*****************************************************************************/
/******* Lock table to make the exchange of questions & answers atomic *********/
/*****************************************************************************/

void FAQ_DB_LockTableQaAs (void)
  {
   DB_Query ("can not lock tables",
	     "LOCK TABLES faq_questions WRITE,"
			 "tre_nodes READ");
   DB_SetThereAreLockedTables ();
  }

/*****************************************************************************/
/********* Update the index of a question & answer given its code **************/
/*****************************************************************************/

void FAQ_DB_UpdateQaAInd (const struct Tre_Node *Node,long QaACod,int ItmInd)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update index of question-answer",
		   "UPDATE faq_questions,"
		          "tre_nodes"
		     " SET faq_questions.ItmInd=%d"
		   " WHERE faq_questions.QaACod=%ld"
		     " AND faq_questions.NodCod=%ld"
		     " AND faq_questions.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   ItmInd,
		   QaACod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_FAQ]);
  }

/*****************************************************************************/
/* Update the question and the answer of a question & answer given its code **/
/*****************************************************************************/

void FAQ_DB_UpdateQaA (const struct Tre_Node *Node)
  {
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];

   DB_QueryUPDATE ("can not update question-answer",
		   "UPDATE faq_questions,"
		          "tre_nodes"
		     " SET faq_questions.Question='%s',"
		          "faq_questions.Answer='%s'"
		   " WHERE faq_questions.QaACod=%ld"
		     " AND faq_questions.NodCod=%ld"
		     " AND faq_questions.NodCod=tre_nodes.NodCod"
		     " AND tre_nodes.CrsCod=%ld"	// Extra check
		     " AND tre_nodes.Type='%s'",	// Extra check
		   Node->QaA.Question,
		   Node->QaA.Answer,
		   Node->SpcItem.Cod,
		   Node->Hierarchy.NodCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Tre_DB_Types[Inf_FAQ]);
  }
