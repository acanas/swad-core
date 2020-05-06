// swad_exam_type.h: definition of types for exams

#ifndef _SWAD_EXA_TYP
#define _SWAD_EXA_TYP
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include "swad_hierarchy.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Exa_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Exa_MAX_BYTES_TITLE	((Exa_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define ExaSet_MAX_CHARS_TITLE	(128 - 1)	// 127
#define ExaSet_MAX_BYTES_TITLE	((ExaSet_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define ExaEvt_MAX_CHARS_TITLE	(128 - 1)	// 127
#define ExaEvt_MAX_BYTES_TITLE	((ExaEvt_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Exa_NUM_ORDERS 3
typedef enum
  {
   Exa_ORDER_BY_START_DATE = 0,
   Exa_ORDER_BY_END_DATE   = 1,
   Exa_ORDER_BY_TITLE      = 2,
  } Exa_Order_t;
#define Exa_ORDER_DEFAULT Exa_ORDER_BY_START_DATE

struct Exa_ExamSelected
  {
   long ExaCod;		// Exam code
   bool Selected;	// Is this exam selected when seeing match results?
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
   long ExaCod;			// Selected/current exam code
   long SetCod;			// Selected/current set code
   long EvtCod;			// Selected/current match code
   unsigned SetInd;		// Current set index
   long QstCod;			// Current question code
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
   bool Hidden;			// Exam is hidden
   unsigned NumSets;		// Number of sets in the exam
   unsigned NumQsts;		// Number of questions in the exam
   unsigned NumEvts;		// Number of events in the exam
   unsigned NumOpenEvts;	// Number of open events in the exam
  };

struct ExaSet_Set
  {
   long ExaCod;			// Exam code
   long SetCod;			// Set code
   unsigned SetInd;		// Set index (position in the exam)
   unsigned NumQstsToExam;	// Number of questions in this set taht will appear in the exam
   char Title[ExaSet_MAX_BYTES_TITLE + 1];	// Title of the set
  };

#define ExaEvt_NUM_SHOWING 5
typedef enum
  {
   ExaEvt_START,	// Start: don't show anything
   ExaEvt_STEM,		// Showing only the question stem
   ExaEvt_ANSWERS,	// Showing the question stem and the answers
   ExaEvt_RESULTS,	// Showing the results
   ExaEvt_END,		// End: don't show anything
  } ExaEvt_Showing_t;
#define ExaEvt_SHOWING_DEFAULT ExaEvt_START

struct ExaEvt_Event
  {
   long EvtCod;
   long ExaCod;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   char Title[ExaEvt_MAX_BYTES_TITLE + 1];
   bool Hidden;
   bool Open;		// If now is between start and end dates
   struct
     {
      unsigned QstInd;	// 0 means that the exam has not started. First question has index 1.
      long QstCod;
      time_t QstStartTimeUTC;
      ExaEvt_Showing_t Showing;	// What is shown on teacher's screen
      long Countdown;		// > 0 ==> countdown in progress
				// = 0 ==> countdown over ==> go to next step
				// < 0 ==> no countdown at this time
      unsigned NumCols;		// Number of columns for answers on teacher's screen
      bool ShowQstResults;	// Show global results of current question while playing
      bool ShowUsrResults;	// Show exam with results of all questions for the student
      bool Happening;		// Is being played now?
      unsigned NumParticipants;
     } Status;			// Status related to event playing
  };

#endif
