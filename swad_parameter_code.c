// swad_parameter_code.c: CGI parameters related to codes

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stddef.h>		// For NULL

#include "swad_error.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Par_CodeStr[Par_NUM_PAR_COD] =
  {
   [ParCod_None     ] = NULL,
   [ParCod_Agd      ] = "AgdCod",	// Agenda event code
   [ParCod_Ann      ] = "AnnCod",	// Announcement code
   [ParCod_Asg      ] = "AsgCod",	// Assignment code
   [ParCod_Att      ] = "AttCod",	// Attendance event code
   [ParCod_Ban      ] = "BanCod",	// Banner code
   [ParCod_Bld      ] = "BldCod",	// Building code
   [ParCod_Cri      ] = "CriCod",	// Criterion code in rubric
   [ParCod_Crs      ] = "crs",		// Course code
   [ParCod_Ctr      ] = "ctr",		// Center code
   [ParCod_Cty      ] = "cty",		// Country code
   [ParCod_Deg      ] = "deg",		// Degree code
   [ParCod_Dpt      ] = "DptCod",	// Department code
   [ParCod_Exa      ] = "ExaCod",	// Exam code in calls-for-exam/exam
   [ParCod_Fil      ] = "FilCod",	// File code
   [ParCod_Fld      ] = "FldCod",	// Field code in course record
   [ParCod_Gam      ] = "GamCod",	// Game code
   [ParCod_Grp      ] = "GrpCod",	// Group code
   [ParCod_GrpTyp   ] = "GrpTypCod",	// Group type code
   [ParCod_Hld      ] = "HldCod",	// Holiday code
   [ParCod_Ins      ] = "ins",		// Institution code
   [ParCod_Itm      ] = "ItmCod",	// Specific item code in course tree
   [ParCod_Lnk      ] = "LnkCod",	// Link code
   [ParCod_Mai      ] = "MaiCod",	// Domain mail code
   [ParCod_Mch      ] = "MchCod",	// Match code in game
   [ParCod_Msg      ] = "MsgCod",	// Message code
   [ParCod_Nod      ] = "NodCod",	// Node code in course program
   [ParCod_Not      ] = "NotCod",	// Note code in timeline
   [ParCod_OrgAct   ] = "OrgActCod",	// Original action code
   [ParCod_OthCrs   ] = "OthCrsCod",	// Other course code
   [ParCod_OthCtr   ] = "OthCtrCod",	// Other center code
   [ParCod_OthCty   ] = "OthCtyCod",	// Other country code
   [ParCod_OthDeg   ] = "OthDegCod",	// Other degree code
   [ParCod_OthDegTyp] = "OthDegTypCod",	// Other degree type code
   [ParCod_OthHie   ] = "OthHieCod",	// Other hierarchy code
   [ParCod_OthIns   ] = "OthInsCod",	// Other institution code
   [ParCod_Plc      ] = "PlcCod",	// Place code
   [ParCod_Plg      ] = "PlgCod",	// Plugin code
   [ParCod_Prj      ] = "PrjCod",	// Project code
   [ParCod_Prn      ] = "PrnCod",	// Print code in assessment test
   [ParCod_Pst      ] = "PstCod",	// Post code in forum
   [ParCod_Pub      ] = "PubCod",	// Publication code in timeline
   [ParCod_Qst      ] = "QstCod",	// Question code in assessment/survey
   [ParCod_Roo      ] = "RooCod",	// Room code
   [ParCod_Rub      ] = "RubCod",	// Rubric code
   [ParCod_Ses      ] = "SesCod",	// Session code in exam
   [ParCod_Set      ] = "SetCod",	// Set of questions code in exam
   [ParCod_Svy      ] = "SvyCod",	// Syrvey code
   [ParCod_Tag      ] = "TagCod",	// Tag code in assessment question
   [ParCod_Thr      ] = "ThrCod",	// Thread code in forum
  };

/*****************************************************************************/
/******************** Get the value of a code parameter **********************/
/*****************************************************************************/

long ParCod_GetAndCheckPar (ParCod_Param_t ParCode)
  {
   return ParCod_GetAndCheckParMin (ParCode,1);
  }

long ParCod_GetAndCheckParMin (ParCod_Param_t ParCode,long MinCodAllowed)
  {
   long Cod;

   /***** Get code and check that is a valid code (>= minimum) *****/
   if ((Cod = ParCod_GetPar (ParCode)) < MinCodAllowed)
      Err_WrongCodeExit ();

   return Cod;
  }

long ParCod_GetPar (ParCod_Param_t ParCode)
  {
   return Par_GetParLong (Par_CodeStr[ParCode]);
  }
