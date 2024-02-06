// swad_exam_type.h: definition of types for exams

#ifndef _SWAD_EXA_TYP
#define _SWAD_EXA_TYP
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#include <stdbool.h>		// For boolean type

#include "swad_date.h"
#include "swad_hierarchy_type.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Exa_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Exa_MAX_BYTES_TITLE	((Exa_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define ExaSet_MAX_CHARS_TITLE	(128 - 1)	// 127
#define ExaSet_MAX_BYTES_TITLE	((ExaSet_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define ExaSes_MAX_CHARS_TITLE	(128 - 1)	// 127
#define ExaSes_MAX_BYTES_TITLE	((ExaSes_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Exa_NUM_ORDERS 3
typedef enum
  {
   Exa_ORDER_BY_START_DATE = 0,
   Exa_ORDER_BY_END_DATE   = 1,
   Exa_ORDER_BY_TITLE      = 2,
  } Exa_Order_t;
#define Exa_ORDER_DEFAULT Exa_ORDER_BY_START_DATE

typedef enum
  {
   Exa_DONT_CHECK_EXA_COD,
   Exa_CHECK_EXA_COD,
  } Exa_CheckExaCod_t;

typedef enum
  {
   Exa_EXISTING_EXAM,
   Exa_NEW_EXAM,
  } Exa_ExistingNewExam_t;

struct Exa_ExamSelected
  {
   long ExaCod;		// Exam code
   bool Selected;	// Is this exam selected when seeing match results?
  };

struct Exa_Exam
  {
   long ExaCod;			// Exam code
   long CrsCod;			// Course code
   long UsrCod;			// Author code
   double MaxGrade;		// Score range [0...max.score]
				// will be converted to
				// grade range [0...max.grade]
   unsigned Visibility;		// Visibility of results
   char Title[Exa_MAX_BYTES_TITLE + 1];
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   HidVis_HiddenOrVisible_t HiddenOrVisible;	// Exam is hidden or visible?
   unsigned NumSets;		// Number of sets in the exam
   unsigned NumQsts;		// Number of questions in the exam
   unsigned NumSess;		// Number of sessions in the exam
   unsigned NumOpenSess;	// Number of open sessions in the exam
  };

/* Exams context */
struct Exa_Exams
  {
   bool LstIsRead;		// Is the list already read from database...
				// ...or it needs to be read?
   unsigned Num;		// Total number of exams
   unsigned NumSelected;	// Number of exams selected
   struct Exa_ExamSelected *Lst;// List of exams
   Exa_Order_t SelectedOrder;
   unsigned CurrentPage;
   char *ListQuestions;
   char *ExaCodsSelected;	// String with selected exam codes separated by separator multiple
   struct Exa_Exam Exam;	// Selected/current exam
   long SetCod;			// Selected/current set code
   long SesCod;			// Selected/current session code
   unsigned SetInd;		// Current set index
   long QstCod;			// Current question code
  };

/*                                                       Student 1
   +----------------+     +-----------+     +--------------+   +--------------+
   |     Exam 1     |--+--| Session 1 |--+--|    Print 1   |   |   Result 1   |
   |+--------------+|  |  | * Start   |  |  | * Question 2 |   | * Question 2 |
   ||     Set 1    ||  |  | * End     |  |  |--------------|-->|--------------|
   || * Question 1 ||  |  | * Groups  |  |  | * Question 5 |   | * Question 5 |
   || * Question 2 ||  |  +-----------+  |  | * Question 3 |   | * Question 3 |
   |+--------------+|  |  +-----------+  |  +--------------+   +--------------+
   |+--------------+|  +--| Session 2 |  |               Student 2
   ||     Set 2    ||     | * Start   |  |  +--------------+   +--------------+
   || * Question 3 ||     | * End     |  +--|    Print 2   |   |   Result 2   |
   || * Question 4 ||     | * Groups  |     | * Question 1 |   | * Question 1 |
   || * Question 5 ||     +-----------+     |--------------|-->|--------------|
   || * Question 6 ||          ...          | * Question 6 |   | * Question 6 |
   |+--------------+|                       | * Question 5 |   | * Question 5 |
   +----------------+                       +--------------+   +--------------+
           ...                                              ...              */

struct ExaSet_Set
  {
   long ExaCod;			// Exam code
   long SetCod;			// Set code
   unsigned SetInd;		// Set index (position in the exam)
   unsigned NumQstsToPrint;	// Number of questions in this set
				// that will appear in each exam print
   char Title[ExaSet_MAX_BYTES_TITLE + 1];	// Title of the set
  };

struct ExaSes_Session
  {
   long SesCod;
   long ExaCod;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   char Title[ExaSes_MAX_BYTES_TITLE + 1];
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   bool Open;		// If now is between start and end dates
   bool ShowUsrResults;	// Show exam with results of all questions for the student
  };

#endif
