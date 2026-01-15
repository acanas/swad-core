// swad_exam_resource.c: links to exams as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_exam_database.h"
#include "swad_exam_resource.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/***************************** Get link to exam ******************************/
/*****************************************************************************/

void ExaRsc_GetLinkToExam (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Exams;
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   Exa_GetPars (&Exams,Exa_DONT_CHECK_EXA_COD);

   /***** Get exam title *****/
   if (Exams.Exam.ExaCod > 0)
      Exa_GetExamDataByCod (&Exams.Exam);

   /***** Copy link to exam into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_EXAM,Exams.Exam.ExaCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Exams.Exam.ExaCod > 0 ? Exams.Exam.Title :
   					  Txt_Exams);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }
