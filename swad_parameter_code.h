// swad_parameter_code.h: CGI parameters related to codes

#ifndef _SWAD_PAR_COD
#define _SWAD_PAR_COD
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/************************** Public types and constants ***********************/
/*****************************************************************************/

typedef enum
  {
   ParCod_None,		// Special code
   ParCod_Agd,		// Agenda event code
   ParCod_Ann,		// Announcement code
   ParCod_Asg,		// Assignment code
   ParCod_Att,		// Attendance event code
   ParCod_Ban,		// Banner code
   ParCod_Bld,		// Building code
   ParCod_Cri,		// Criterion code in rubric
   ParCod_Crs,		// Course code
   ParCod_Ctr,		// Center code
   ParCod_Cty,		// Country code
   ParCod_Deg,		// Degree code
   ParCod_Dpt,		// Department code
   ParCod_Exa,		// Exam code in calls-for-exam/exam
   ParCod_Fil,		// File code
   ParCod_Fld,		// Field code in course record
   ParCod_Gam,		// Game code
   ParCod_Grp,		// Group code
   ParCod_GrpTyp,	// Group type code
   ParCod_Hld,		// Holiday code
   ParCod_Ins,		// Institution code
   ParCod_Nod,		// Item code in course program
   ParCod_Lnk,		// Link code
   ParCod_Mai,		// Domain mail code
   ParCod_Mch,		// Match code in game
   ParCod_Msg,		// Message code
   ParCod_Not,		// Note code in timeline
   ParCod_OrgAct,	// Original action code
   ParCod_OthCrs,	// Other course code
   ParCod_OthCtr,	// Other center code
   ParCod_OthCty,	// Other country code
   ParCod_OthDeg,	// Other degree code
   ParCod_OthDegTyp,	// Other degree type code
   ParCod_OthHie,	// Other hierarchy code
   ParCod_OthIns,	// Other institution code
   ParCod_Plc,		// Place code
   ParCod_Plg,		// Plugin code
   ParCod_Prj,		// Project code
   ParCod_Prn,		// Print code in assessment test
   ParCod_Pst,		// Post code in forum
   ParCod_Pub,		// Publication code in timeline
   ParCod_QaA,		// Question&answer code in FAQ
   ParCod_Qst,		// Question code in assessment/survey
   ParCod_Roo,		// Room code
   ParCod_Rsc,		// Resource code in course program
   ParCod_Rub,		// Rubric code
   ParCod_Ses,		// Session code in exam
   ParCod_Set,		// Set of questions code in exam
   ParCod_Svy,		// Syrvey code
   ParCod_Tag,		// Tag code in assessment question
   ParCod_Thr,		// Thread code in forum
  } ParCod_Param_t;
#define Par_NUM_PAR_COD ((unsigned) ParCod_Thr + 1)

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------ Get parameters -------------------------------
long ParCod_GetAndCheckPar (ParCod_Param_t ParCod);
long ParCod_GetAndCheckParMin (ParCod_Param_t ParCod,long MinCodAllowed);
long ParCod_GetPar (ParCod_Param_t ParCod);

//------------------------------ Put parameters -------------------------------
void ParCod_PutPar (ParCod_Param_t ParCod,long Cod);

#endif
