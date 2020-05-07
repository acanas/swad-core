// swad_exam_print.c: exam prints (each copy of an exam in an event for a student)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_exam_event.h"
#include "swad_exam_result.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct ExaPrn_Print
  {
   unsigned foo;
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaPrn_PrintConstructor (struct ExaPrn_Print *Print);
static void ExaPrn_PrintDestructor (struct ExaPrn_Print *Print);

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct Exa_Exam *Exam,
	                                          struct ExaPrn_Print *Print);

/*****************************************************************************/
/******************* Generate print of an exam in an event *******************/
/*****************************************************************************/

void ExaPrn_ShowNewExamPrint (void)
  {
   extern const char *Hlp_ASSESSMENT_Exams;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   struct ExaPrn_Print Print;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Create print *****/
   ExaPrn_PrintConstructor (&Print);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Exam.Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (false,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** Get questions from database *****/
   ExaPrn_GetQuestionsForNewPrintFromDB (&Exam,&Print);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Destroy print *****/
   ExaPrn_PrintDestructor (&Print);
  }

/*****************************************************************************/
/***************************** Print constructor *****************************/
/*****************************************************************************/

static void ExaPrn_PrintConstructor (struct ExaPrn_Print *Print)
  {
   Print->foo = 1;
  }

/*****************************************************************************/
/****************************** Print destructor *****************************/
/*****************************************************************************/

static void ExaPrn_PrintDestructor (struct ExaPrn_Print *Print)
  {
   Print->foo = 1;
  }

/*****************************************************************************/
/*********** Get questions for a new exam print from the database ************/
/*****************************************************************************/

#define ExaPrn_MAX_BYTES_QUERY_PRINT (16 * 1024 - 1)

static void ExaPrn_GetQuestionsForNewPrintFromDB (struct Exa_Exam *Exam,
	                                          struct ExaPrn_Print *Print)
  {
   extern const char *Txt_question;
   extern const char *Txt_questions;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSets;
   unsigned NumSet;
   struct ExaSet_Set Set;

   Print->foo = 1;

   /***** Get data of set of questions from database *****/
   NumSets = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get sets of questions",
			      "SELECT SetCod,"		// row[0]
				     "NumQstsToPrint,"	// row[1]
				     "Title"		// row[2]
			      " FROM exa_sets"
			      " WHERE ExaCod=%ld"
			      " ORDER BY SetInd",
			      Exam->ExaCod);

   /***** Show table with sets *****/
   if (NumSets)
      /***** Write rows *****/
      for (NumSet = 0;
	   NumSet < NumSets;
	   NumSet++)
	{
	 Gbl.RowEvenOdd = NumSet % 2;

	 /***** Create set of questions *****/
	 ExaSet_ResetSet (&Set);

	 /***** Get set data *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] SetCod
	 row[1] NumQstsToPrint
	 row[2] Title
	 */
	 /* Get set code (row[0]) */
	 Set.SetCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get set index (row[1]) */
	 Set.NumQstsToPrint = Str_ConvertStrToUnsigned (row[1]);

	 /* Get the title of the set (row[2]) */
	 Str_Copy (Set.Title,row[2],
		   ExaSet_MAX_BYTES_TITLE);

	 /***** Begin row for this set *****/
	 HTM_TR_Begin (NULL);

	 /***** Title *****/
	 HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Set.Title);
	 HTM_TD_End ();

	 /***** Number of questions to appear in exam print *****/
	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Unsigned (Set.NumQstsToPrint);
	 HTM_NBSP ();
	 HTM_Txt (Set.NumQstsToPrint == 1 ? Txt_question :
		                            Txt_questions);
	 HTM_TD_End ();

	 /***** End first row *****/
	 HTM_TR_End ();
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
