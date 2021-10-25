// swad_question.c: test/exam/game questions

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
/*********************************** Headers *********************************/
/*****************************************************************************/

// #define _GNU_SOURCE 		// For asprintf
// #include <limits.h>		// For UINT_MAX
// #include <linux/limits.h>	// For PATH_MAX
// #include <mysql/mysql.h>	// To access MySQL databases
// #include <stdbool.h>		// For boolean type
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For exit, system, malloc, free, etc
// #include <string.h>		// For string functions
// #include <sys/stat.h>		// For mkdir
// #include <sys/types.h>		// For mkdir

// #include "swad_action.h"
// #include "swad_box.h"
// #include "swad_database.h"
// #include "swad_error.h"
// #include "swad_exam_set.h"
// #include "swad_figure.h"
// #include "swad_form.h"
// #include "swad_global.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
// #include "swad_language.h"
// #include "swad_match.h"
// #include "swad_media.h"
// #include "swad_parameter.h"
#include "swad_question.h"
// #include "swad_question_import.h"
// #include "swad_tag_database.h"
// #include "swad_test.h"
// #include "swad_test_config.h"
// #include "swad_test_print.h"
// #include "swad_test_visibility.h"
// #include "swad_theme.h"
// #include "swad_user.h"
// #include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************** Change format of answers text / feedback ******************/
/*****************************************************************************/

void Qst_ChangeFormatAnswersText (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      /* Convert answer text, that is in HTML, to rigorous HTML */
      if (Question->Answer.Options[NumOpt].Text[0])
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Question->Answer.Options[NumOpt].Text,
			   Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
  }

void Qst_ChangeFormatAnswersFeedback (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text and feedback *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      /* Convert answer feedback, that is in HTML, to rigorous HTML */
      if (Question->Answer.Options[NumOpt].Feedback)
	 if (Question->Answer.Options[NumOpt].Feedback[0])
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Question->Answer.Options[NumOpt].Feedback,
			      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
  }
